#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <chrono>
#include "String.h"
using namespace std;
using namespace std::chrono;

struct Time {
    system_clock::time_point tp;
    Time(const String& timeStr);
};

struct Timestamp {
    system_clock::time_point tp;
    Timestamp(const String& timeStr);
};

class TimeUtils {
private:
    // static String _formatDateTime(tm* timeinfo, int extra, const char* format);
    TimeUtils() = default;
    virtual ~TimeUtils() = default;
public:
    static system_clock::time_point getNow();
    static system_clock::time_point getTime(long milli);
    static long getEpochMillis(system_clock::time_point& t1);
    static time_t getEpochSecs(system_clock::time_point& t1);
    static time_t mkgmtime(struct tm *tm);
    static system_clock::time_point parseLocalTime(const char* timestr);
    static system_clock::time_point parseTimestamp(const char* timestamp);
    static system_clock::time_point parseLocalDate(const char* datestr);
    static String getLocalTime(long milli, const char* format, bool addMillis = true);
    static String getLocalTime(system_clock::time_point& t1, const char* format, bool addMillis = true);
    static String getGmTime(long milli, const char* format, bool getMillis = true);
    static String getGmTime(system_clock::time_point& t1, const char* format, bool getMillis = true);
    static String getUTCTimestamp(system_clock::time_point& t1);
};

#endif