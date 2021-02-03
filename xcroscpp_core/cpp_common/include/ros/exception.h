#ifndef _XCROS_CORE_EXCEPTION_H_
#define _XCROS_CORE_EXCEPTION_H_

#include <stdexcept>

namespace xcros
{
    //todo: learn std::runtime_error
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