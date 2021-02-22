#ifndef _XCROS_CORE_TIME_H_
#define _XCROS_CORE_TIME_H_

#include "ros/exception.h"

#include <math.h>

#include "rostime_decl.h"
#include "duration.h"
#include <boost/date_time/posix_time/ptime.hpp>

namespace xcros
{
    class ROSTIME_DECL TimeNotInitializedException : public Exception
    {
    private:
        typedef Exception Super;

    public:
        TimeNotInitializedException()
            :Super("Cannot use ros::Time::now() before the first NodeHandle has been created or ros::start() has been called.  "
                  "If this is a standalone app or test that just uses ros::Time and does not communicate over ROS, you may also call ros::Time::init()")
            {}


    };

    class ROSTIME_DECL NoHighPerformanceTimerException : public Exception
    {
    private:
        typedef Exception Super;
    
    public:
        NoHighPerformanceTimerException()
            :Super("This windows platform does not "
                  "support the high-performance timing api.")
            {}
    };

/*********************************************************************
 ** Functions
 *********************************************************************/

  ROSTIME_DECL void normalizeSecNSec(uint64_t& sec, uint64_t& nsec);
  ROSTIME_DECL void normalizeSecNSec(uint32_t& sec, uint32_t& nsec);
  ROSTIME_DECL void normalizeSecNSecUnsigned(int64_t& sec, int64_t& nsec);
  ROSTIME_DECL void ros_walltime(uint32_t& sec, uint32_t& nsec);
  ROSTIME_DECL void ros_steadytime(uint32_t& sec, uint32_t& nsec);

/*********************************************************************
 ** Time Classes
 *********************************************************************/
  template<class T, class D>
  class TimeBase
  {
  public:
    uint32_t sec, nsec;

    TimeBase() : sec(0), nsec(0) { }
    TimeBase(uint32_t _sec, uint32_t _nsec) : sec(_sec), nsec(_nsec)
    {
      normalizeSecNSec(sec, nsec);
    }
    explicit TimeBase(double t) { fromSec(t); }
    D operator-(const T &rhs) const;
    T operator+(const D &rhs) const;
    T operator-(const D &rhs) const;
    T& operator+=(const D &rhs);
    T& operator-=(const D &rhs);
    bool operator==(const T &rhs) const;
    inline bool operator!=(const T &rhs) const { return !(*static_cast<const T*>(this) == rhs); }
    bool operator>(const T &rhs) const;
    bool operator<(const T &rhs) const;
    bool operator>=(const T &rhs) const;
    bool operator<=(const T &rhs) const;

    double toSec()  const { return (double)sec + 1e-9*(double)nsec; };
    T& fromSec(double t) {
      int64_t sec64 = (int64_t)floor(t);
      if (sec64 < 0 || sec64 > UINT_MAX)
        throw std::runtime_error("Time is out of dual 32-bit range");
      sec = (uint32_t)sec64;
      nsec = (uint32_t)round((t-sec) * 1e9);
      // avoid rounding errors
      sec += (nsec / 1000000000ul);
      nsec %= 1000000000ul;
      return *static_cast<T*>(this);
    }

    uint64_t toNSec() const {return (uint64_t)sec*1000000000ull + (uint64_t)nsec;  }
    T& fromNSec(uint64_t t);

    inline bool isZero() const { return sec == 0 && nsec == 0; }
    inline bool is_zero() const { return isZero(); }
    boost::posix_time::ptime toBoost() const;

  };

  /**
   * \brief Time representation.  May either represent wall clock time or ROS clock time.
   *
   * ros::TimeBase provides most of its functionality.
   */
  class ROSTIME_DECL Time : public TimeBase<Time, Duration>
  {
  public:
    Time()
      : TimeBase<Time, Duration>()
    {}

    Time(uint32_t _sec, uint32_t _nsec)
      : TimeBase<Time, Duration>(_sec, _nsec)
    {}

    explicit Time(double t) { fromSec(t); }

    /**
     * \brief Retrieve the current time.  If ROS clock time is in use, this returns the time according to the
     * ROS clock.  Otherwise returns the current wall clock time.
     */
    static Time now();
    /**
     * \brief Sleep until a specific time has been reached.
     * @return True if the desired sleep time was met, false otherwise.
     */
    static bool sleepUntil(const Time& end);

    static void init();
    static void shutdown();
    static void setNow(const Time& new_now);
    static bool useSystemTime();
    static bool isSimTime();
    static bool isSystemTime();

    /**
     * \brief Returns whether or not the current time is valid.  Time is valid if it is non-zero.
     */
    static bool isValid();
    /**
     * \brief Wait for time to become valid
     */
    static bool waitForValid();
    /**
     * \brief Wait for time to become valid, with timeout
     */
    static bool waitForValid(const xcros::WallDuration& timeout);

    static Time fromBoost(const boost::posix_time::ptime& t);
    static Time fromBoost(const boost::posix_time::time_duration& d);
  };

  extern ROSTIME_DECL const Time TIME_MAX;
  extern ROSTIME_DECL const Time TIME_MIN;

  /**
   * \brief Time representation.  Always wall-clock time.
   *
   * ros::TimeBase provides most of its functionality.
   */
  class ROSTIME_DECL WallTime : public TimeBase<WallTime, WallDuration>
  {
  public:
    WallTime()
      : TimeBase<WallTime, WallDuration>()
    {}

    WallTime(uint32_t _sec, uint32_t _nsec)
      : TimeBase<WallTime, WallDuration>(_sec, _nsec)
    {}

    explicit WallTime(double t) { fromSec(t); }

    /**
     * \brief Returns the current wall clock time.
     */
    static WallTime now();

    /**
     * \brief Sleep until a specific time has been reached.
     * @return True if the desired sleep time was met, false otherwise.
     */
    static bool sleepUntil(const WallTime& end);

    static bool isSystemTime() { return true; }
  };

  /**
   * \brief Time representation.  Always steady-clock time.
   *
   * Not affected by ROS time.
   *
   * ros::TimeBase provides most of its functionality.
   */
  class ROSTIME_DECL SteadyTime : public TimeBase<SteadyTime, WallDuration>
  {
    public:
      SteadyTime()
        : TimeBase<SteadyTime, WallDuration>()
      {}

      SteadyTime(uint32_t _sec, uint32_t _nsec)
        : TimeBase<SteadyTime, WallDuration>(_sec, _nsec)
      {}

      explicit SteadyTime(double t) { fromSec(t); }

      /**
       * \brief Returns the current steady (monotonic) clock time.
       */
      static SteadyTime now();

      /**
       * \brief Sleep until a specific time has been reached.
       * @return True if the desired sleep time was met, false otherwise.
       */
      static bool sleepUntil(const SteadyTime& end);

      static bool isSystemTime() { return true; }
  };

  ROSTIME_DECL std::ostream &operator <<(std::ostream &os, const Time &rhs);
  ROSTIME_DECL std::ostream &operator <<(std::ostream &os, const WallTime &rhs);
  ROSTIME_DECL std::ostream &operator <<(std::ostream &os, const SteadyTime &rhs);

};


#endif