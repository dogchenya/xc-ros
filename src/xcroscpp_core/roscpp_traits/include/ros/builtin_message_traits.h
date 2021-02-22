

#ifndef _XCROS_CORE_BUILTIN_MESSAGE_TRAITS_H
#define _XCROS_CORE_BUILTIN_MESSAGE_TRAITS_H

#include "ros/message_traits.h"
#include "ros/time.h"

namespace xcros
{
namespace message_traits
{

#define ROSLIB_CREATE_SIMPLE_TRAITS(Type) \
    template<> struct IsSimple<Type> : public TrueType {}; \
    template<> struct IsFixedSize<Type> : public TrueType {};

ROSLIB_CREATE_SIMPLE_TRAITS(uint8_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int8_t)
ROSLIB_CREATE_SIMPLE_TRAITS(uint16_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int16_t)
ROSLIB_CREATE_SIMPLE_TRAITS(uint32_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int32_t)
ROSLIB_CREATE_SIMPLE_TRAITS(uint64_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int64_t)
ROSLIB_CREATE_SIMPLE_TRAITS(float)
ROSLIB_CREATE_SIMPLE_TRAITS(double)
ROSLIB_CREATE_SIMPLE_TRAITS(Time)
ROSLIB_CREATE_SIMPLE_TRAITS(Duration)

// because std::vector<bool> is not a true vector, bool is not a simple type
template<> struct IsFixedSize<bool> : public TrueType {};

} // namespace message_traits
} // namespace xcros

#endif // ROSLIB_BUILTIN_MESSAGE_TRAITS_H

