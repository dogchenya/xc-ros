
#ifndef _XCROSTIME_IMPL_DURATION_H_INCLUDED_
#define _XCROSTIME_IMPL_DURATION_H_INCLUDED_

#include <ros/duration.h>
#include <ros/rate.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace xcros {
  //
  // DurationBase template member function implementation
  //
  template<class T>
  DurationBase<T>::DurationBase(int32_t _sec, int32_t _nsec)
  : sec(_sec), nsec(_nsec)
  {
    normalizeSecNSecSigned(sec, nsec);
  }

  template<class T>
  T& DurationBase<T>::fromSec(double d)
  {
    int64_t sec64 = (int64_t)floor(d);
    if (sec64 < INT_MIN || sec64 > INT_MAX)
      throw std::runtime_error("Duration is out of dual 32-bit range");
    sec = (int32_t)sec64;
    nsec = (int32_t)(nearbyint((d - (double)sec)*1000000000));
    return *static_cast<T*>(this);
  }

  template<class T>
  T& DurationBase<T>::fromNSec(int64_t t)
  {
    int64_t sec64 = t / 1000000000;
    if (sec64 < INT_MIN || sec64 > INT_MAX)
      throw std::runtime_error("Duration is out of dual 32-bit range");
    sec = (int32_t)sec64;
    nsec = (int32_t)(t % 1000000000);
    return *static_cast<T*>(this);
  }

  template<class T>
  T DurationBase<T>::operator+(const T &rhs) const
  {
    T t;
    return t.fromNSec(toNSec() + rhs.toNSec());
  }

  template<class T>
  T DurationBase<T>::operator*(double scale) const
  {
    return T(toSec() * scale);
  }

  template<class T>
  T DurationBase<T>::operator-(const T &rhs) const
  {
    T t;
    return t.fromNSec(toNSec() - rhs.toNSec());
  }

  template<class T>
  T DurationBase<T>::operator-() const
  {
    T t;
    return t.fromNSec(-toNSec());
  }

  template<class T>
  T& DurationBase<T>::operator+=(const T &rhs)
  {
    *this = *this + rhs;
    return *static_cast<T*>(this);
  }

  template<class T>
  T& DurationBase<T>::operator-=(const T &rhs)
  {
    *this += (-rhs);
    return *static_cast<T*>(this);
  }

  template<class T>
  T& DurationBase<T>::operator*=(double scale)
  {
    fromSec(toSec() * scale);
    return *static_cast<T*>(this);
  }

  template<class T>
  bool DurationBase<T>::operator<(const T &rhs) const
  {
    if (sec < rhs.sec)
      return true;
    else if (sec == rhs.sec && nsec < rhs.nsec)
      return true;
    return false;
  }

  template<class T>
  bool DurationBase<T>::operator>(const T &rhs) const
  {
    if (sec > rhs.sec)
      return true;
    else if (sec == rhs.sec && nsec > rhs.nsec)
      return true;
    return false;
  }

  template<class T>
  bool DurationBase<T>::operator<=(const T &rhs) const
  {
    if (sec < rhs.sec)
      return true;
    else if (sec == rhs.sec && nsec <= rhs.nsec)
      return true;
    return false;
  }

  template<class T>
  bool DurationBase<T>::operator>=(const T &rhs) const
  {
    if (sec > rhs.sec)
      return true;
    else if (sec == rhs.sec && nsec >= rhs.nsec)
      return true;
    return false;
  }

  template<class T>
  bool DurationBase<T>::operator==(const T &rhs) const
  {
    return sec == rhs.sec && nsec == rhs.nsec;
  }

  template<class T>
  bool DurationBase<T>::isZero() const
  {
    return sec == 0 && nsec == 0;
  }

  template <class T>
  boost::posix_time::time_duration
  DurationBase<T>::toBoost() const
  {
    namespace bt = boost::posix_time;
#if defined(BOOST_DATE_TIME_HAS_NANOSECONDS)
    return bt::seconds(sec) + bt::nanoseconds(nsec);
#else
    return bt::seconds(sec) + bt::microseconds(nsec/1000);
#endif
  }
}
#endif
