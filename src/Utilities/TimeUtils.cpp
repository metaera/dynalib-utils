#include <iomanip>
#include <chrono>
using namespace std;
using namespace std::chrono;

#include "TimeUtils.h"

Time::Time(const String& timeStr) {
    tp = TimeUtils::parseLocalTime(timeStr.c_str());
}

Timestamp::Timestamp(const String& timeStr) {
    tp = TimeUtils::parseTimestamp(timeStr.c_str());
}

system_clock::time_point TimeUtils::getNow() {
    return system_clock::now();
}

system_clock::time_point TimeUtils::getTime(long milli) {
    return system_clock::from_time_t(milli);
}

time_t TimeUtils::getEpochSecs(system_clock::time_point& t1) {
    return system_clock::to_time_t(t1);
}

long TimeUtils::getEpochMillis(system_clock::time_point& t1) {
    return duration_cast<milliseconds>(t1.time_since_epoch()).count();    
}

time_t TimeUtils::mkgmtime(struct tm *tm) {
    auto t = mktime(tm);
    return t + (mktime(localtime(&t)) - mktime(gmtime(&t)));
}

system_clock::time_point TimeUtils::parseLocalTime(const char* timestr) {
    stringstream str( timestr );
    tm tm = {};
    str.imbue( locale() );
    str >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    tm.tm_isdst = -1;
    return getTime(mktime(&tm));
}

system_clock::time_point TimeUtils::parseTimestamp(const char* timestamp) {
    stringstream str( timestamp );
    tm tm = {};
    str >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    tm.tm_isdst = 0;
    return getTime(mkgmtime(&tm));
}

system_clock::time_point TimeUtils::parseLocalDate(const char* datestr) {
    stringstream str( datestr );
    tm tm = {};
    str.imbue( locale() );
    str >> get_time(&tm, "%Y-%m-%d");
    tm.tm_isdst = -1;
    return getTime(mktime(&tm));
}

// String TimeUtils::_formatDateTime(tm* timeinfo, int extra, const char* format) {
//     char   buf[80];
//     strftime(buf, sizeof(buf), format, timeinfo);
//     char   buf2[84];
//     if (extra > 0) {
//         sprintf(buf2, "%s.%03d", buf, extra);
//     }
//     else {
//         sprintf(buf2, "%s", buf);
//     }
//     return String(buf2);
// }

String TimeUtils::getLocalTime(long milli, const char* format, bool addMillis) {
    ostringstream ss;
    tm tm = {};
    time_t secs = milli / 1000;
    localtime_r(&secs, &tm);
    tm.tm_isdst = 0;
    ss << put_time(&tm, format);
    if (addMillis) {
        auto ms = milli % 1000;
        ss << '.' << std::setfill('0') << std::setw(3) << ms;
    }
    return ss.str();
}

String TimeUtils::getLocalTime(system_clock::time_point& t1, const char* format, bool addMillis) {
    return getLocalTime(getEpochMillis(t1), format, addMillis);
}

String TimeUtils::getGmTime(long milli, const char* format, bool addMillis) {
    ostringstream ss;
    tm tm = {};
    time_t secs = milli / 1000;
    gmtime_r(&secs, &tm);
    tm.tm_isdst = 0;
    ss << put_time(&tm, format);
    if (addMillis) {
        auto ms = milli % 1000;
        ss << '.' << std::setfill('0') << std::setw(3) << ms;
    }
    return ss.str();
}

String TimeUtils::getGmTime(system_clock::time_point& t1, const char* format, bool addMillis) {
    return getGmTime(getEpochMillis(t1), format, addMillis);
}

String TimeUtils::getUTCTimestamp(system_clock::time_point& t1) {
    return getGmTime(getEpochMillis(t1), "%FT%T", true) + "Z";
}
