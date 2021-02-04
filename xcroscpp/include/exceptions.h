#ifndef _XCROSCPP_EXCEPTIONS_H_
#define _XCROSCPP_EXCEPTIONS_H_

#include <ros/exception.h>

namespace xcros
{

/**
 * \brief Thrown when an invalid node name is specified to ros::init()
 */
class InvalidNodeNameException : public xcros::Exception
{
public:
  InvalidNodeNameException(const std::string& name, const std::string& reason)
  : Exception("Invalid node name [" + name + "]: " + reason)
  {}
};

/**
 * \brief Thrown when an invalid graph resource name is specified to any roscpp
 * function.
 */
class InvalidNameException : public xcros::Exception
{
public:
  InvalidNameException(const std::string& msg)
  : Exception(msg)
  {}
};

/**
 * \brief Thrown when a second (third,...) subscription is attempted with conflicting
 * arguments.
 */
class ConflictingSubscriptionException : public xcros::Exception
{
public:
  ConflictingSubscriptionException(const std::string& msg)
  : Exception(msg)
  {}
};

/**
 * \brief Thrown when an invalid parameter is passed to a method
 */
class InvalidParameterException : public xcros::Exception
{
public:
  InvalidParameterException(const std::string& msg)
  : Exception(msg)
  {}
};

/**
 * \brief Thrown when an invalid port is specified
 */
class InvalidPortException : public xcros::Exception
{
public:
  InvalidPortException(const std::string& msg)
  : Exception(msg)
  {}
};

}

#endif