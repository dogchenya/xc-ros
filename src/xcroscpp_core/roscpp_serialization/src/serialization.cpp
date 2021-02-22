#include <ros/serialization.h>

namespace xcros
{
namespace serialization
{
void throwStreamOverrun()
{
  throw StreamOverrunException("Buffer Overrun");
}
}
}