#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include <unistd.h>
#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include "Timestamp.h"


namespace CurrentThread {
    extern __thread int t_cachedTid;
    extern __thread const char *t_threadName;
    
    inline pid_t gettid() {
        return static_cast<pid_t> (::syscall(SYS_gettid));
    }

    inline void cacheTid() {
        if (t_cachedTid == 0) {
            t_cachedTid = gettid();
        }
    }
    
    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cacheTid();
        }
        return t_cachedTid;
    }


    inline bool isMainThread() {
        return tid() == gettid();
    }

	
	inline void sleepUsec(int64_t usec)
	{
		  struct timespec ts = { 0, 0 };
		    ts.tv_sec = static_cast<time_t>(usec / Timestamp::M);
			  ts.tv_nsec = static_cast<long>(usec % Timestamp::M * 1000);
			    ::nanosleep(&ts, NULL);
	}
}

#endif
