#ifndef _XCROS_CORE_EXCEPTION_H_
#define _XCROS_CORE_EXCEPTION_H_

#include <stdexcept>

namespace xcros
{
    //todo: learn std::runtime_error
    //std::runtime_error : 运行时错误异常类，它的唯一目的时充当异常结构中的基类的类
    //与std::logic_error 区别
    //异常存在多态 catch时只会 捕获特定异常
    class Exception : public std::runtime_error
    {
    private:
        typedef std::runtime_error Super;
    public:
        Exception(const std::string& what)
                : Super(what){};
    };
}




#endif