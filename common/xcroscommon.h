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

#include "boost/shared_ptr.hpp"
#include <boost/make_shared.hpp>

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

    template<typename T>
    class SingletonPtr
    {
    public:
        static boost::shared_ptr<T>& Instance()
        {
            static boost::shared_ptr<T> m_instance = boost::make_shared<T>();
            return m_instance;
        }

    protected:
        SingletonPtr(void){};
        virtual ~SingletonPtr(){};

    private:
        SingletonPtr(const SingletonPtr& rhs){}
        SingletonPtr& operator = (const SingletonPtr& rhs){}

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