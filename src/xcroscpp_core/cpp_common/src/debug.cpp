#include "ros/debug.h"

#if defined(HAVE_EXECINFO_H)
#include <execinfo.h>
#endif

#if defined(HAVE_CXXABI_H)
#include <cxxabi.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <sstream>

namespace xcros
{
namespace debug
{

void getBacktrace(V_void& addresses)
{
#if HAVE_GLIBC_BACKTRACE
  void *array[64];

  size_t size = backtrace(array, 64);
  for (size_t i = 1; i < size; i++)
  {
    addresses.push_back(array[i]);
  }
#endif
}

void translateAddresses(const V_void& addresses, V_string& lines)
{
#if HAVE_GLIBC_BACKTRACE
  if (addresses.empty())
  {
    return;
  }

  size_t size = addresses.size();
  char **strings = backtrace_symbols(&addresses.front(), size);

  for (size_t i = 0; i < size; ++i)
  {
    lines.push_back(strings[i]);
  }

  free(strings);
#endif
}

std::string demangleName(const std::string& name)
{
#if HAVE_CXXABI_H
  int status;
  char* demangled = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
  std::string out;
  if (demangled)
  {
    out = demangled;
    free(demangled);
  }
  else
  {
    out = name;
  }

  return out;
#else
  return name;
#endif
}

std::string demangleBacktraceLine(const std::string& line)
{
  // backtrace_symbols outputs in the form:
  // executable(function+offset) [address]
  // We want everything between ( and + to send to demangleName()
  size_t paren_pos = line.find('(');
  size_t plus_pos = line.find('+');
  if (paren_pos == std::string::npos || plus_pos == std::string::npos)
  {
    return line;
  }

  std::string name(line, paren_pos + 1, plus_pos - paren_pos - 1);
  return line.substr(0, paren_pos + 1) + demangleName(name) + line.substr(plus_pos);
}

void demangleBacktrace(const V_string& lines, V_string& demangled)
{
  V_string::const_iterator it = lines.begin();
  V_string::const_iterator end = lines.end();
  for (; it != end; ++it)
  {
    demangled.push_back(demangleBacktraceLine(*it));
  }
}

std::string backtraceToString(const V_void& addresses)
{
  V_string lines, demangled;
  translateAddresses(addresses, lines);
  demangleBacktrace(lines, demangled);

  std::stringstream ss;
  V_string::const_iterator it = demangled.begin();
  V_string::const_iterator end = demangled.end();
  for (; it != end; ++it)
  {
    ss << *it << std::endl;
  }

  return ss.str();
}

std::string getBacktrace()
{
  V_void addresses;
  getBacktrace(addresses);
  return backtraceToString(addresses);
}

}

}
