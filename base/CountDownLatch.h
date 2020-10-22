#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H
#include "Condition.h"
#include "Mutex.h"


class CountDownLatch {
public:
    explicit CountDownLatch(int);
    void wait();
    void countDown();
    int getCount() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;    
    int count_;
};


#endif
