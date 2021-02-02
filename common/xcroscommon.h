#ifndef _XCROS_COMMON_H_
#define _XCROS_COMMON_H_

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
}


#endif