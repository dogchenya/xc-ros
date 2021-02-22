#ifndef _XCROS_CORE_PALTFORM_
#define _XCROS_CORE_PALTFORM_

#ifndef _MSC_VER
  #include <stdlib.h> // getenv
#endif
#include <string>

namespace xcros {

/**
 * Convenient cross platform function for returning a std::string of an
 * environment variable.
 */
inline bool get_environment_variable(std::string &str, const char* environment_variable) {
	char* env_var_cstr = NULL;
	#ifdef _MSC_VER
	  _dupenv_s(&env_var_cstr, NULL,environment_variable);
	#else
	  env_var_cstr = getenv(environment_variable);
	#endif
	if ( env_var_cstr ) {
		str = std::string(env_var_cstr);
        #ifdef _MSC_VER
		  free(env_var_cstr);
        #endif
		return true;
	} else {
		str = std::string("");
		return false;
	}
}

} // namespace xcros


#endif
