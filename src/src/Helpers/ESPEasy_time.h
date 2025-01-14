#ifndef HELPERS_ESPEASY_TIME_H
#define HELPERS_ESPEASY_TIME_H

#include "../../ESPEasy_common.h"

#include "../DataTypes/ESPEasyTimeSource.h"

#include <time.h>


class ESPEasy_time {
public:

  ESPEasy_time();

  struct tm addSeconds(const struct tm& ts,
                       int              seconds,
                       bool             toLocalTime,
                       bool             fromLocalTime = false) const;

  // Restore the last known system time
  // This may be useful to get some idea of what time it is.
  // This way the unit can do things based on local time even when NTP servers may not respond.
  // Do not use this when booting from deep sleep.
  // Only call this once during boot.
  void restoreFromRTC();

  // Restore the last known system time
  // This may be useful to get some idea of what time it is.
  // This way the unit can do things based on local time even when NTP servers may not respond.
  // Do not use this when booting from deep sleep.
  // Only call this once during boot.
  void restoreLastKnownUnixTime(unsigned long lastSysTime,
                                uint8_t       deepSleepState);

  // Set external time source
  // Wander is expected error in msec
  bool setExternalTimeSource_withTimeWander(double       new_time,
                                            timeSource_t new_timeSource,
                                            int32_t      wander,
                                            uint8_t      unitnr = 0);

  bool setExternalTimeSource(double       new_time,
                             timeSource_t new_timeSource,
                             uint8_t      unitnr = 0);

  uint32_t getUptime_in_sec() const;

  // Get unix time in seconds
  uint32_t getUnixTime() const;

  // Get unix time in seconds
  // @param unix_time_frac  The fractional part
  uint32_t getUnixTime(uint32_t& unix_time_frac) const;

  // Convert the UnixTime to systemMicros
  // Returns converted system micros when system time has been set.
  // Returns UnixTime in usec when system time has not yet been set, so it can be easily identified.
  // Return value is negative if Unix timestamp was before system boot.
  int64_t Unixtime_to_systemMicros(const uint32_t& unix_time_sec,
                                   uint32_t        unix_time_frac = 0) const;

  // Convert the system micros() to Unix Time.
  // Return value is in seconds.
  // Returns UnixTime when time has been set, otherwise the seconds part of system micros
  uint32_t systemMicros_to_Unixtime(const int64_t& systemMicros,
                                    uint32_t     & unix_time_frac) const;

  // Convert the system micros() to Unix Time.
  // Return value is in seconds.
  // Returns LocalTime when time has been set, otherwise the seconds part of system micros
  uint32_t      systemMicros_to_Localtime(const int64_t& systemMicros,
                                          uint32_t     & unix_time_frac) const;

  void          initTime();

  unsigned long getLocalUnixTime() const;
  unsigned long getLocalUnixTime(uint32_t& unix_time_frac) const;

  // Update and get the current systime
  unsigned long now_();

  // Update time and return whether the minute has changed since last check.
  bool          reportNewMinute();

  bool          systemTimePresent() const;

  bool          getNtpTime(double& unixTime_d);

  String        getTimeZoneOffsetString();

  void          applyTimeZone();

  /********************************************************************************************\
     Date/Time string formatters
   \*********************************************************************************************/

public:

  // Format the current Date separated by the given delimiter
  // Default date format example: 20161231 (YYYYMMDD)
  String getDateString(char delimiter = '\0') const;

  // Formats the current Time
  // Default time format example: 235959 (HHMMSS)
  String getTimeString(char delimiter    = '\0',
                       bool show_seconds = true,
                       char hour_prefix  = '\0') const;

  String getTimeString_ampm(char delimiter    = '\0',
                            bool show_seconds = true,
                            char hour_prefix  = '\0') const;


  String getDateTimeString(char dateDelimiter          = '-',
                           char timeDelimiter          = ':',
                           char dateTimeDelimiter      = ' ') const;
  String getDateTimeString_ampm(char dateDelimiter     = '-',
                                char timeDelimiter     = ':',
                                char dateTimeDelimiter = ' ') const;


  /********************************************************************************************\
     Get current time/date
   \*********************************************************************************************/

  // Get the year given a Unix time stamp
  static int    year(unsigned long t);

  // Get the weekday, given a Unix time stamp
  static int    weekday(unsigned long t);

  // Convert a weekday number (Sun = 1 ... Sat = 7) to a 3 letter string
  static String weekday_str(int wday);

  // Convert a month number (Jan = 1 ... Dec = 12) to a 3 letter string
  static String month_str(int month);


  // Get current year.
  int year() const
  {
    return 1900 + local_tm.tm_year;
  }

  // Get current month
  uint8_t month() const
  {
    return local_tm.tm_mon + 1; // tm_mon starts at 0
  }

  // Get current day of the month
  uint8_t day() const
  {
    return local_tm.tm_mday;
  }

  // Get current hour
  uint8_t hour() const
  {
    return local_tm.tm_hour;
  }

  // Get current minute
  uint8_t minute() const
  {
    return local_tm.tm_min;
  }

  // Get current second
  uint8_t second() const
  {
    return local_tm.tm_sec;
  }

  // day of week, sunday is day 1
  int weekday() const
  {
    return local_tm.tm_wday;
  }

  String weekday_str() const;

  String month_str() const;


  /********************************************************************************************\
     Sunrise/Sunset calculations
   \*********************************************************************************************/

public:

  // Compute the offset in seconds of the substring +/-<nn>[smh]
  static int getSecOffset(const String& format);
  String     getSunriseTimeString(char delimiter) const;
  String     getSunsetTimeString(char delimiter) const;
  String     getSunriseTimeString(char delimiter,
                                  int  secOffset) const;
  String     getSunsetTimeString(char delimiter,
                                 int  secOffset) const;

private:

  static float sunDeclination(int doy);
  static float diurnalArc(float dec,
                          float lat);
  static float equationOfTime(int doy);
  static int   dayOfYear(int year,
                         int month,
                         int day);

  void      calcSunRiseAndSet(bool timeSynced);
  struct tm getSunRise(int secOffset) const;
  struct tm getSunSet(int secOffset) const;

#if FEATURE_EXT_RTC

public:

  bool ExtRTC_get(uint32_t& unixtime);

private:

  bool ExtRTC_set(uint32_t unixtime);
#endif // if FEATURE_EXT_RTC

public:

  timeSource_t getTimeSource() const { return _timeSource; }
  

  struct tm local_tm;                         // local time
  uint32_t syncInterval = 3600;               // time sync will be attempted after this many seconds

  uint64_t unixTime_usec_uptime_offset = 0.0; // Use usec resolution to get better sync between nodes when using NTP
  uint32_t nextSyncTime                = 0;   // Next time to allow time sync against UNIX time (thus seconds)
  uint32_t lastSyncTime_ms             = 0;
  uint32_t lastNTPSyncTime_ms          = 0;
  int64_t externalUnixTime_offset_usec{};     // Computed offset from current systime
  struct tm tsRise, tsSet;
  struct tm sunRise;
  struct tm sunSet;
private:
  timeSource_t _timeSource              = timeSource_t::No_time_source;
  timeSource_t extTimeSource            = timeSource_t::No_time_source;
public:
  float timeWander                      = 0.0f; // Clock instability in ppm
  uint32_t lastTimeWanderCalculation_ms = 0;

  uint8_t PrevMinutes         = 0;
  uint8_t timeSource_p2p_unit = 0;
};


#endif // HELPERS_ESPEASY_TIME_H
