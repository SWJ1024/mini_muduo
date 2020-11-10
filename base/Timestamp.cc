#include "Timestamp.h"
#include <sys/time.h>
#include <inttypes.h>

Timestamp::Timestamp() : microSeconds(0) {}


Timestamp::Timestamp(int64_t t) : microSeconds(t) {}

void Timestamp::swap(Timestamp &t) {
    std::swap(t.microSeconds, microSeconds);
}

string Timestamp::toString() const {
    char buf[32]={0};
    int64_t s = microSeconds / M;
    int64_t us = microSeconds % M;
    //snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", s, ms);
    snprintf(buf, sizeof(buf), "%ld.%ld", s, us);
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t second = tv.tv_sec;
    return Timestamp(second*Timestamp::M+tv.tv_usec);   
}

string Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t s = static_cast<time_t> (microSeconds / M);
    struct tm tm_time;
    gmtime_r(&s, &tm_time);
    if (showMicroseconds) {
        int m = static_cast<int>(microSeconds % M);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                m);
    }
    else {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
}

int64_t Timestamp::getmicroSecond() const {
    return microSeconds;
}


time_t Timestamp::getSecond() const {
    return microSeconds / Timestamp::M;
}
