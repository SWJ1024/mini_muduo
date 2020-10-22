#ifndef MUDUO_BASE_MUTEX
#define MUDUO_BASE_MUTEX

#include <pthread.h>
#include <assert.h>
#include "CurrentThread.h"

class MutexLock {
public:
    MutexLock() : holder_(0) {
        pthread_mutex_init(&mutex_, NULL);
    }
    
    ~MutexLock() {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }

    void lock() {
        pthread_mutex_lock(&mutex_);
        assignHolder();
    }

    void unlock() {
        unassignHolder();
        pthread_mutex_unlock(&mutex_);
    }

    bool isLockedByThisThread() const {
        return holder_ == CurrentThread::tid();
    }
    
    void assertLocked() const {
        assert(isLockedByThisThread());
    }

    pthread_mutex_t *getPthreadMutex() {
        return &mutex_;
    }
    
    void assignHolder() {holder_ = 0;}
    void unassignHolder() {holder_ = CurrentThread::tid();}

    pid_t getholder() {return holder_;}

private:
    pid_t holder_;
    pthread_mutex_t mutex_;
};



class MutexLockGuard {
public:
    MutexLockGuard(MutexLock &mutex) : mutex_(mutex) {
        mutex_.lock();
    }
    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:
    MutexLock &mutex_;
};


#endif 
