#ifndef _XCROS_CONSOLE_BACKEND_H_
#define _XCROS_CONSOLE_BACKEND_H_

namespace xcros
{
namespace console
{

namespace levels
{
enum Level
{
  Debug,
  Info,
  Warn,
  Error,
  Fatal,

  Count
};
}
typedef levels::Level Level;

namespace backend
{

void notifyLoggerLevelsChanged();

extern void (*function_notifyLoggerLevelsChanged)();

void print(void* logger_handle, ::xcros::console::Level level, const char* str, const char* file, const char* function, int line);

extern void (*function_print)(void*, ::xcros::console::Level, const char*, const char*, const char*, int);

} // namespace backend
} // namespace console
} // namespace ros

#endif