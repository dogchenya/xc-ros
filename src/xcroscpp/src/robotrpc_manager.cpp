#include "ros/robotrpc_manager.h"

#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread> 

#include "RobotRpc.h"
#include "ros/duration.h"
#include "ros/network.h"

using namespace RobotRpc;

namespace xcros
{
    namespace robotrpc
    {
        RobotRpc::XmlRpcValue responseStr(int code, const std::string& msg, const std::string& response)
        {
            RobotRpc::XmlRpcValue v;
            v[0] = code;
            v[1] = msg;
            v[2] = response;
            return v;
        }

        RobotRpc::XmlRpcValue responseInt(int code, const std::string& msg, int response)
        {
            RobotRpc::XmlRpcValue v;
            v[0] = int(code);
            v[1] = msg;
            v[2] = response;
            return v;
        }

        RobotRpc::XmlRpcValue responseBool(int code, const std::string& msg, bool response)
        {
            RobotRpc::XmlRpcValue v;
            v[0] = int(code);
            v[1] = msg;
            v[2] = RobotRpc::XmlRpcValue(response);
            return v;
        }
    }

    class RobotRPCCallWrapper : public RobotRpc::RobotRpcServerMethod
    {
    public:
        RobotRPCCallWrapper(const std::string& function_name, const XMLRPCFunc& cb, RobotRpcServer *s)
        : RobotRpc::RobotRpcServerMethod(function_name, s)
        , name_(function_name)
        , func_(cb)
        { }

        void execute(RobotRpc::XmlRpcValue &params, RobotRpc::XmlRpcValue &result)
        {
            func_(params, result);
        }

    private:
    std::string name_;
    XMLRPCFunc func_;
    };

    void getPid(const RobotRpc::XmlRpcValue& params, RobotRpc::XmlRpcValue& result)
    {
        (void)params;
        result = robotrpc::responseInt(1, "", (int)getpid());
    }

//class rpc manager
    RobotRPCManager::RobotRPCManager()
    : port_(0)
    , shutting_down_(false)
    , unbind_requested_(false)
    {
    }

    RobotRPCManager::~RobotRPCManager()
    {
        shutdown();
    }

    void RobotRPCManager::start()
    {
    shutting_down_ = false;
    port_ = 0;
    bind("getPid", getPid);

    bool bound = server_.bindAndListen(0);
    (void) bound;
    
    assert(bound);
    port_ = server_.get_port();
    assert(port_ != 0);

    std::stringstream ss;
    ss << "http://" << network::getHost() << ":" << port_ << "/";
    uri_ = ss.str();

    server_thread_ = std::thread(std::bind(&RobotRPCManager::serverThreadFunc, this));
    }

    void RobotRPCManager::shutdown()
    {
    if (shutting_down_)
    {
        return;
    }

    shutting_down_ = true;
    server_thread_.join();

    server_.close();

    // kill the last few clients that were started in the shutdown process
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);

        for (V_CachedRobotRpcClient::iterator i = clients_.begin();
            i != clients_.end();)
        {
        if (!i->in_use_)
        {
            i->client_->close();
            delete i->client_;
            i = clients_.erase(i);
        }
        else
        {
            ++i;
        }
        }
    }

    // Wait for the clients that are in use to finish and remove themselves from clients_
    for (int wait_count = 0; !clients_.empty() && wait_count < 10; wait_count++)
    {
        //ROSCPP_LOG_DEBUG("waiting for xmlrpc connection to finish...");
        xcros::WallDuration(0.01).sleep();
    }

    std::lock_guard<std::mutex> lock(functions_mutex_);
    functions_.clear();

    {
        S_ASyncXMLRPCConnection::iterator it = connections_.begin();
        S_ASyncXMLRPCConnection::iterator end = connections_.end();
        for (; it != end; ++it)
        {
        (*it)->removeFromDispatch(server_.get_dispatch());
        }
    }

    connections_.clear();

    {
        std::lock_guard<std::mutex> lock(added_connections_mutex_);
        added_connections_.clear();
    }

    {
        std::lock_guard<std::mutex> lock(removed_connections_mutex_);
        removed_connections_.clear();
    }
    }

    bool RobotRPCManager::validateXmlrpcResponse(const std::string& method, RobotRpc::XmlRpcValue &response,
                                        RobotRpc::XmlRpcValue &payload)
    {
    if (response.getType() != RobotRpc::XmlRpcValue::TypeArray)
    {
        //ROSCPP_LOG_DEBUG("XML-RPC call [%s] didn't return an array",
        //    method.c_str());
        return false;
    }
    if (response.size() != 2 && response.size() != 3)
    {
        //ROSCPP_LOG_DEBUG("XML-RPC call [%s] didn't return a 2 or 3-element array",
        //    method.c_str());
        return false;
    }
    if (response[0].getType() != RobotRpc::XmlRpcValue::TypeInt)
    {
        //ROSCPP_LOG_DEBUG("XML-RPC call [%s] didn't return a int as the 1st element",
        //    method.c_str());
        return false;
    }
    int status_code = response[0];
    if (response[1].getType() != RobotRpc::XmlRpcValue::TypeString)
    {
        //ROSCPP_LOG_DEBUG("XML-RPC call [%s] didn't return a string as the 2nd element",
        //    method.c_str());
        return false;
    }
    std::string status_string = response[1];
    if (status_code != 1)
    {
        //ROSCPP_LOG_DEBUG("XML-RPC call [%s] returned an error (%d): [%s]",
        //    method.c_str(), status_code, status_string.c_str());
        return false;
    }
    if (response.size() > 2)
    {
        payload = response[2];
    }
    else
    {
        std::string empty_array = "<value><array><data></data></array></value>";
        int offset = 0;
        payload = RobotRpc::XmlRpcValue(empty_array, &offset);
    }
    return true;
    }

    void RobotRPCManager::serverThreadFunc()
    {
    xcros::disableAllSignalsInThisThread();

    while(!shutting_down_)
    {
        {
        std::lock_guard<std::mutex> lock(added_connections_mutex_);
        S_ASyncXMLRPCConnection::iterator it = added_connections_.begin();
        S_ASyncXMLRPCConnection::iterator end = added_connections_.end();
        for (; it != end; ++it)
        {
            (*it)->addToDispatch(server_.get_dispatch());
            connections_.insert(*it);
        }

        added_connections_.clear();
        }

        // Update the XMLRPC server, blocking for at most 100ms in select()
        {
        std::lock_guard<std::mutex> lock(functions_mutex_);
        server_.work(0.1);
        }

        while (unbind_requested_)
        {
        WallDuration(0.01).sleep();
        }

        if (shutting_down_)
        {
        return;
        }

        {
        S_ASyncXMLRPCConnection::iterator it = connections_.begin();
        S_ASyncXMLRPCConnection::iterator end = connections_.end();
        for (; it != end; ++it)
        {
            if ((*it)->check())
            {
            removeASyncConnection(*it);
            }
        }
        }

        {
        std::lock_guard<std::mutex> lock(removed_connections_mutex_);
        S_ASyncXMLRPCConnection::iterator it = removed_connections_.begin();
        S_ASyncXMLRPCConnection::iterator end = removed_connections_.end();
        for (; it != end; ++it)
        {
            (*it)->removeFromDispatch(server_.get_dispatch());
            connections_.erase(*it);
        }

        removed_connections_.clear();
        }
    }
    }

    RobotRpcClient* RobotRPCManager::getRobotRPCClient(const std::string &host, const int port, const std::string &uri)
    {
    // go through our vector of clients and grab the first available one
    RobotRpcClient *c = NULL;

    std::lock_guard<std::mutex> lock(clients_mutex_);

    for (V_CachedRobotRpcClient::iterator i = clients_.begin();
        !c && i != clients_.end(); )
    {
        if (!i->in_use_)
        {
        // see where it's pointing
        if (i->client_->getHost() == host &&
            i->client_->getPort() == port &&
            i->client_->getUri()  == uri)
        {
            // hooray, it's pointing at our destination. re-use it.
            c = i->client_;
            i->in_use_ = true;
            i->last_use_time_ = SteadyTime::now();
            break;
        }
        else if (i->last_use_time_ + CachedRobotRpcClient::s_zombie_time_ < SteadyTime::now())
        {
            // toast this guy. he's dead and nobody is reusing him.
            delete i->client_;
            i = clients_.erase(i);
        }
        else
        {
            ++i; // move along. this guy isn't dead yet.
        }
        }
        else
        {
        ++i;
        }
    }

    if (!c)
    {
        // allocate a new one
        c = new RobotRpcClient(host.c_str(), port, uri.c_str());
        CachedRobotRpcClient mc(c);
        mc.in_use_ = true;
        mc.last_use_time_ = SteadyTime::now();
        clients_.push_back(mc);
        //ROS_INFO("%d xmlrpc clients allocated\n", xmlrpc_clients.size());
    }
    // ONUS IS ON THE RECEIVER TO UNSET THE IN_USE FLAG
    // by calling releaseRobotRpcClient
    return c;
    }

    void RobotRPCManager::releaseRobotRPCClient(RobotRpcClient *c)
    {
    std::lock_guard<std::mutex> lock(clients_mutex_);

    for (V_CachedRobotRpcClient::iterator i = clients_.begin();
        i != clients_.end(); ++i)
    {
        if (c == i->client_)
        {
        if (shutting_down_)
        {
            // if we are shutting down we won't be re-using the client
            i->client_->close();
            delete i->client_;
            clients_.erase(i);
        }
        else
        {
            i->in_use_ = false;
        }
        break;
        }
    }
    }

    void RobotRPCManager::addASyncConnection(const ASyncXMLRPCConnectionPtr& conn)
    {
    std::lock_guard<std::mutex> lock(added_connections_mutex_);
    added_connections_.insert(conn);
    }

    void RobotRPCManager::removeASyncConnection(const ASyncXMLRPCConnectionPtr& conn)
    {
    std::lock_guard<std::mutex> lock(removed_connections_mutex_);
    removed_connections_.insert(conn);
    }

    bool RobotRPCManager::bind(const std::string& function_name, const XMLRPCFunc& cb)
    {
    std::lock_guard<std::mutex> lock(functions_mutex_);
    if (functions_.find(function_name) != functions_.end())
    {
        return false;
    }

    FunctionInfo info;
    info.name = function_name;
    info.function = cb;
    info.wrapper.reset(new RobotRPCCallWrapper(function_name, cb, &server_));
    functions_[function_name] = info;

    return true;
    }

    void RobotRPCManager::unbind(const std::string& function_name)
    {
    unbind_requested_ = true;
    std::lock_guard<std::mutex> lock(functions_mutex_);
    functions_.erase(function_name);
    unbind_requested_ = false;
    }

//end
}