#ifndef _XCROSCPP_RPC_MANAGER_H_
#define _XCROSCPP_RPC_MANAGER_H_

#include <thread>
#include <mutex>
#include <set>
#include "boost/enable_shared_from_this.hpp"

#include "common/xcroscommon.h"

#include "ros/xcroscpp_common.h"
#include "ros/duration.h"
#include "ros/time.h"

#include "robotrpcpp/include/RobotRpc.h"

namespace xcros
{
    namespace robotrpc
    {
        RobotRpc::XmlRpcValue responseStr(int code, const std::string& msg, const std::string& response);
        RobotRpc::XmlRpcValue responseInt(int code, const std::string& msg, int response);
        RobotRpc::XmlRpcValue responseBool(int code, const std::string& msg, bool response);
    }

    class RobotRPCCallWrapper;
    typedef boost::shared_ptr<RobotRPCCallWrapper> RobotRPCCallWrapperPtr;

//class ASyncXMLRPCConnection
    class ASyncXMLRPCConnection : public boost::enable_shared_from_this<ASyncXMLRPCConnection>
    {
    public:
        virtual ~ASyncXMLRPCConnection() {}

        virtual void addToDispatch(RobotRpc::RobotRpcDispatch* disp) = 0;
        virtual void removeFromDispatch(RobotRpc::RobotRpcDispatch* disp) = 0;

        virtual bool check() = 0;
    };

    typedef boost::shared_ptr<ASyncXMLRPCConnection> ASyncXMLRPCConnectionPtr;
    typedef std::set<ASyncXMLRPCConnectionPtr> S_ASyncXMLRPCConnection;
//end

    class CachedRobotRpcClient
    {
    public:
        CachedRobotRpcClient(RobotRpc::RobotRpcClient *c)
        : in_use_(false)
        , client_(c)
        {
        }

    bool in_use_;
    xcros::SteadyTime last_use_time_; // for reaping
    RobotRpc::RobotRpcClient* client_;

    static const xcros::WallDuration s_zombie_time_; // how long before it is toasted
    };

//class RobotRPCManager
    class RobotRPCManager;
    typedef boost::shared_ptr<RobotRPCManager> RobotRPCManagerPtr;

    typedef std::function<void(RobotRpc::XmlRpcValue&, RobotRpc::XmlRpcValue&)> XMLRPCFunc;

    class RobotRPCManager : public Singleton<RobotRPCManager>
    {

    public:
        RobotRPCManager();
        ~RobotRPCManager();

        bool validateXmlrpcResponse(const std::string& method, 
			      RobotRpc::XmlRpcValue &response, RobotRpc::XmlRpcValue &payload);

        inline const std::string& getServerURI() const { return uri_; }
        
        inline uint32_t getServerPort() const { return port_; }

        RobotRpc::RobotRpcClient* getRobotRPCClient(const std::string& host, const int port, const std::string& uri);
        void releaseRobotRPCClient(RobotRpc::RobotRpcClient* c);

        void addASyncConnection(const ASyncXMLRPCConnectionPtr& conn);
        void removeASyncConnection(const ASyncXMLRPCConnectionPtr& conn);

        bool bind(const std::string& function_name, const XMLRPCFunc& cb);
        void unbind(const std::string& function_name);

        void start();
        void shutdown();

        bool isShuttingDown() { return shutting_down_; }

    private:
        void serverThreadFunc();

        bool shutting_down_;
        std::string uri_;
        int port_;
        std::thread server_thread_;

        RobotRpc::RobotRpcServer server_;
        typedef std::vector<CachedRobotRpcClient> V_CachedRobotRpcClient;
        V_CachedRobotRpcClient clients_;
        std::mutex clients_mutex_;
        
        xcros::WallDuration master_retry_timeout_;

        S_ASyncXMLRPCConnection added_connections_;
        std::mutex added_connections_mutex_;

        S_ASyncXMLRPCConnection removed_connections_;
        std::mutex removed_connections_mutex_;

        S_ASyncXMLRPCConnection connections_;


        struct FunctionInfo
        {
            std::string name;
            XMLRPCFunc function;
            RobotRPCCallWrapperPtr wrapper;
        };
        typedef std::map<std::string, FunctionInfo> M_StringToFuncInfo;
        std::mutex functions_mutex_;
        M_StringToFuncInfo functions_;

        volatile bool unbind_requested_;
    };

}
//class end

#endif