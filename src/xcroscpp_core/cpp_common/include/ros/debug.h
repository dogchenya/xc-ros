#ifndef _XCROSLIB_DEBUG_H_
#define _XCROSLIB_DEBUG_H_

#include <string>
#include <vector>
#include "cpp_common_decl.h"

namespace xcros
{

namespace debug
{
typedef std::vector<void*> V_void;
typedef std::vector<std::string> V_string;

CPP_COMMON_DECL std::string getBacktrace();
CPP_COMMON_DECL std::string backtraceToString(const V_void& addresses);
CPP_COMMON_DECL void getBacktrace(V_void& addresses);
CPP_COMMON_DECL void translateAddresses(const V_void& addresses, V_string& lines);
CPP_COMMON_DECL void demangleBacktrace(const V_string& names, V_string& demangled);
CPP_COMMON_DECL std::string demangleBacktrace(const V_string& names);
CPP_COMMON_DECL std::string demangleName(const std::string& name);
}

}

#endif
