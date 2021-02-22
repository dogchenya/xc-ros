#ifndef _XCROS_CORE_DATATYPES_H_
#define _XCROS_CORE_DATATYPES_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

//#include <memory>
#include "boost/shared_ptr.hpp"

namespace xcros
{
    typedef std::vector<std::pair<std::string, std::string> > VP_string;
    typedef std::vector<std::string> V_string;
    typedef std::set<std::string> S_string;
    typedef std::map<std::string, std::string> M_string;
    typedef std::pair<std::string, std::string> StringPair;

    typedef boost::shared_ptr<M_string> M_stringPtr;
}

#endif