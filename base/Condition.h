#ifndef MUDUO_BASE_CONDITION_H
#define MUDUO_BASE_CONDITION_H

#include "Mutex.h"
#include <pthread.h>

class Condition {
public:
    explicit Condition(MutexLock &m) : mutex_(m) {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition() {
        pthread_cond_destroy(&pcond_);
    }

    void wait() {
       mutex_.unassignHolder();
       pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()); 
    }

    void notify() {
        pthread_cond_signal(&pcond_);
    }

    void notifyall() {
        pthread_cond_broadcast(&pcond_);
    }
private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};



#endif
