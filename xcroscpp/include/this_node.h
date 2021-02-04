#ifndef _XCROSCPP_THIS_NODE_H_
#define _XCROSCPP_THIS_NODE_H_

#include "xcroscpp_common.h"
#include "forwards.h"

namespace xcros
{

/**
 * \brief Contains functions which provide information about this process' ROS node
 */
namespace this_node
{

/**
 * \brief Returns the name of the current node.
 */
ROSCPP_DECL const std::string& getName();
/**
 * \brief Returns the namespace of the current node.
 */
ROSCPP_DECL const std::string& getNamespace();

/** @brief Get the list of topics advertised by this node
 *
 * @param[out] topics The advertised topics
 */
ROSCPP_DECL void getAdvertisedTopics(V_string& topics);

/** @brief Get the list of topics subscribed to by this node
 *
 * @param[out] The subscribed topics
 */
ROSCPP_DECL void getSubscribedTopics(V_string& topics);

} // namespace this_node

} // namespace xcros

#endif