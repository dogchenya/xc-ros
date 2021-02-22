#include "ros/topic.h"
#include "ros/callback_queue.h"

namespace xcros
{

namespace Topic
{

void waitForMessageImpl(SubscribeOptions& ops,
            const boost::function<bool(void)>& ready_pred,
            NodeHandle& nh, xcros::Duration timeout)
{
    xcros::CallbackQueue queue;
    ops.callback_queue = &queue;

    xcros::Subscriber sub = nh.subscribe(ops);

    xcros::Time end = xcros::Time::now() + timeout;
    while(!ready_pred() && nh.ok())
    {
        queue.callAvailable(xcros::WallDuration(0.1));

        if(!timeout.isZero() && xcros::Time::now() >= end)
        {
            return;
        }
    }
}

}

}
