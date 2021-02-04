#ifndef _XCROS_COMMON_H_
#define _XCROS_COMMON_H_


#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include <sys/types.h>
#if !defined(WIN32)
#include <unistd.h>
#include <pthread.h>
#endif
#include <signal.h>

namespace xcros
{
    template<typename T>
    class Singleton
    {

    public:
        static T& Instance()
        {
            static T m_instance;
            return m_instance;
        }

    protected:
        Singleton(void){};
        virtual ~Singleton(){};

    private:
        Singleton(const Singleton& rhs){}

        Singleton& operator = (const Singleton& rhs){}

        static T volatile m_instance; //2.为什么加上volatile关键字 --> 确保多线程每次从内存中取值，而不是从寄存器中取值

    };

    // using std::string;

    // void disableAllSignalsInThisThread()
    // {
    // #if !defined(WIN32)
    // // pthreads_win32, despite having an implementation of pthread_sigmask,
    // // doesn't have an implementation of sigset_t, and also doesn't expose its
    // // pthread_sigmask externally.
    // sigset_t signal_set;

    // /* block all signals */
    // sigfillset( &signal_set );
    // pthread_sigmask( SIG_BLOCK, &signal_set, NULL );
    // #endif
    // }
}


#endif