#ifndef _XCROSCPP_CALLBACK_QUEUE_INTERFACE_H_
#define _XCROSCPP_CALLBACK_QUEUE_INTERFACE_H_

#include <memory>
#include "xcroscpp_common.h"
#include "ros/types.h"

namespace xcros
{

class ROSCPP_DECL CallbackInterface
{
public:

    enum CallResult
    {
        Success,
        TryAgain,
        Invalid,
    };

    virtual ~CallbackInterface(){}

    virtual CallResult call() = 0;
    /**
     * \brief Provides the opportunity for specifying that a callback is not ready to be called
     * before call() actually takes place.
     */
    virtual bool ready() { return true; }
};

typedef std::shared_ptr<CallbackInterface> CallbackInterfacePtr;

class ROSCPP_DECL CallbackQueueInterface
{
public:
    virtual ~CallbackQueueInterface(){}

    virtual void addCallback(const CallbackInterfacePtr& callback, uint64_t owner_id = 0) = 0;

    virtual void removeByID(uint64_t owner_id) = 0;
};

}


#endif