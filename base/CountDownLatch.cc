#include "CountDownLatch.h"


CountDownLatch::CountDownLatch(int cnt)
    : mutex_(),
      condition_(mutex_),
      count_(cnt)
{
}

void CountDownLatch::wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0) {
        condition_.wait();
    }
}

void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0) {
        condition_.notifyall();
    }
}

int CountDownLatch::getCount() const {
    MutexLockGuard lock(mutex_);
    return count_;
}

