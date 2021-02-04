#ifndef _XCROSCPP_NAMES_H_
#define _XCROSCPP_NAMES_H_

#include "forwards.h"
#include "xcroscpp_common.h"

namespace xcros
{

/**
 * \brief Contains functions which allow you to manipulate ROS names
 */
namespace names
{

/**
 * \brief Cleans a graph resource name: removes double slashes, trailing slash
 */
ROSCPP_DECL std::string clean(const std::string& name);
/**
 * \brief Resolve a graph resource name into a fully qualified graph resource name
 *
 * See http://www.ros.org/wiki/Names for more details
 *
 * \param name Name to resolve
 * \param remap Whether or not to apply remappings to the name
 * \throws InvalidNameException if the name passed is not a valid graph resource name
 */
ROSCPP_DECL std::string resolve(const std::string& name, bool remap = true);
/**
 * \brief Resolve a graph resource name into a fully qualified graph resource name
 *
 * See http://www.ros.org/wiki/Names for more details
 *
 * \param ns Namespace to use in resolution
 * \param name Name to resolve
 * \param remap Whether or not to apply remappings to the name
 * \throws InvalidNameException if the name passed is not a valid graph resource name
 */
ROSCPP_DECL std::string resolve(const std::string& ns, const std::string& name, bool remap = true);
/**
 * \brief Append one name to another
 */
ROSCPP_DECL std::string append(const std::string& left, const std::string& right);
/**
 * \brief Apply remappings to a name
 * \throws InvalidNameException if the name passed is not a valid graph resource name
 */
ROSCPP_DECL std::string remap(const std::string& name);
/**
 * \brief Validate a name against the name spec
 */
ROSCPP_DECL bool validate(const std::string& name, std::string& error);

ROSCPP_DECL const M_string& getRemappings();
ROSCPP_DECL const M_string& getUnresolvedRemappings();

/**
 * \brief Get the parent namespace of a name
 * \param name The namespace of which to get the parent namespace.  
 * \throws InvalidNameException if the name passed is not a valid graph resource name
 */
ROSCPP_DECL std::string parentNamespace(const std::string& name);

} // namespace names

} // namespace ros

#endif