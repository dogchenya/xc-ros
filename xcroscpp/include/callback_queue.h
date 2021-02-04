#ifndef _XCROSCPP_CALLBACK_QUEUE_H_
#define _XCROSCPP_CALLBACK_QUEUE_H_

#ifdef BOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
#include <boost/version.hpp>
#if BOOST_VERSION < 106100
// use backported version of boost condition variable, see https://svn.boost.org/trac/boost/ticket/6377
//#include "boost_161_condition_variable.h"
#else // Boost version is 1.61 or greater and has the steady clock fixes
#include <boost/thread/condition_variable.hpp>
#endif
#else // !BOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
#include <boost/thread/condition_variable.hpp>
#endif // BOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC

#include "xcroscpp_common.h"
#include "callback_queue_interface.h"
#include "ros/time.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/tss.hpp>

#include <list>
#include <deque>

namespace xcros
{
    class ROSCPP_DECL CallbackQueue : public CallbackQueueInterface
    {
    public:
        CallbackQueue(bool enabled = true);

        virtual ~CallbackQueue();

        virtual void addCallback(const CallbackInterfacePtr& callback, uint64_t removal_id = 0);

        virtual void removeByID(uint64_t removal_id);

        enum CallOneResult
        {
            Called,
            TryAgain,
            Disabled,
            Empty,
        };

        CallOneResult callOne()
        {
            return callOne(xcros::WallDuration());
        }

        CallOneResult callOne(xcros::WallDuration timeout);

        void callAvailable()
        {
            callAvailable(xcros::WallDuration());
        }

         void callAvailable(xcros::WallDuration timeout);

        /**
         * \brief returns whether or not the queue is empty
         */
        bool empty() { return isEmpty(); }
        /**
         * \brief returns whether or not the queue is empty
         */
        bool isEmpty();
        /**
         * \brief Removes all callbacks from the queue.  Does \b not wait for calls currently in progress to finish.
         */
        void clear();

        /**
         * \brief Enable the queue (queue is enabled by default)
         */
        void enable();
        /**
         * \brief Disable the queue, meaning any calls to addCallback() will have no effect
         */
        void disable();
        /**
         * \brief Returns whether or not this queue is enabled
         */
        bool isEnabled();

    protected:
        void setupTLS();

        struct TLS;
        CallOneResult callOneCB(TLS* tls);

        struct IDInfo
        {
            uint64_t id;
            boost::shared_mutex calling_rw_mutex;
        };
        typedef std::shared_ptr<IDInfo> IDInfoPtr;
        typedef std::map<uint64_t, IDInfoPtr> M_IDInfo;

        IDInfoPtr getIDInfo(uint64_t id);

        struct CallbackInfo
        {
            CallbackInfo()
            : removal_id(0)
            , marked_for_removal(false)
            {}
            CallbackInterfacePtr callback;
            uint64_t removal_id;
            bool marked_for_removal;
        };
        typedef std::list<CallbackInfo> L_CallbackInfo;
        typedef std::deque<CallbackInfo> D_CallbackInfo;
        D_CallbackInfo callbacks_;
        size_t calling_;
        boost::mutex mutex_;
        boost::condition_variable condition_;

        boost::mutex id_info_mutex_;
        M_IDInfo id_info_;

        struct TLS
        {
            TLS()
            : calling_in_this_thread(0xffffffffffffffffULL)
            , cb_it(callbacks.end())
            {}
            uint64_t calling_in_this_thread;
            D_CallbackInfo callbacks;
            D_CallbackInfo::iterator cb_it;
        };
        boost::thread_specific_ptr<TLS> tls_;

        bool enabled_;

    };

    typedef std::shared_ptr<CallbackQueue> CallbackQueuePtr;
}

#endif