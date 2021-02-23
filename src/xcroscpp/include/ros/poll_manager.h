#ifndef _XCROS_POLL_MANAGER_H_
#define _XCROS_POLL_MANAGER_H_

#include <memory>
#include <thread>

#include <boost/signals2.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>

#include "common/xcroscommon.h"
#include "ros/io.h"
#include "ros/poll_set.h"


namespace xcros
{
    class PollMangaer;
    typedef boost::shared_ptr<PollMangaer> PollMangaerPtr;
    typedef boost::signals2::signal<void(void)> VoidSignal;
    typedef boost::function<void(void)> VoidFunc;

    class PollManager : public SingletonPtr<PollManager>
    {
    public:
        PollManager();
        ~PollManager();

        PollSet& getPollSet() { return poll_set_; }

        boost::signals2::connection addPollThreadListener(const VoidFunc& func);
        void removePollThreadListener(boost::signals2::connection c);

        void start();
        void shutdown();

    private:
        void threadFunc();

        PollSet poll_set_;
        volatile bool shutting_down_;

        VoidSignal poll_signal_;
        boost::recursive_mutex signal_mutex_;

        boost::thread thread_;
    };
}

#endif