

#ifndef _XCROS_CORE_SERVICE_TRAITS_H_
#define _XCROS_CORE_SERVICE_TRAITS_H_

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>

namespace xcros
{
namespace service_traits
{

/**
 * \brief Specialize to provide the md5sum for a service
 */
template<typename M>
struct MD5Sum
{
  static const char* value()
  {
    return M::__s_getServerMD5Sum().c_str();
  }

  static const char* value(const M& m)
  {
    return m.__getServerMD5Sum().c_str();
  }
};

/**
 * \brief Specialize to provide the datatype for a service
 */
template<typename M>
struct DataType
{
  static const char* value()
  {
    return M::__s_getServiceDataType().c_str();
  }

  static const char* value(const M& m)
  {
    return m.__getServiceDataType().c_str();
  }
};

/**
 * \brief return MD5Sum<M>::value();
 */
template<typename M>
inline const char* md5sum()
{
  return MD5Sum<typename boost::remove_reference<typename boost::remove_const<M>::type>::type>::value();
}

/**
 * \brief return DataType<M>::value();
 */
template<typename M>
inline const char* datatype()
{
  return DataType<typename boost::remove_reference<typename boost::remove_const<M>::type>::type>::value();
}

/**
 * \brief return MD5Sum<M>::value(m);
 */
template<typename M>
inline const char* md5sum(const M& m)
{
  return MD5Sum<typename boost::remove_reference<typename boost::remove_const<M>::type>::type>::value(m);
}

/**
 * \brief return DataType<M>::value();
 */
template<typename M>
inline const char* datatype(const M& m)
{
  return DataType<typename boost::remove_reference<typename boost::remove_const<M>::type>::type>::value(m);
}

} // namespace service_traits
} // namespace ros

#endif // ROSCPP_SERVICE_TRAITS_H
