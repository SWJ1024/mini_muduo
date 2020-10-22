#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include <unistd.h>
#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>


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
}

#endif
