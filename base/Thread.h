#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include <pthread.h>
#include "Atomic.h"
#include <functional>
#include <string>
#include "CountDownLatch.h"
using std::string;

class Thread {
public:
    using ThreadFunc = std::function<void()>; 
    explicit Thread(ThreadFunc, const string &name = string());
    ~Thread();
    void start();
    int join();

    bool isStart() {return start_;}
    pid_t getTid() const {return tid_;}
    const string& getName() const {return name_;}
    static int getNumCreated();
    void setDefaultName();
    void run() {func_();}
    void setTid(pid_t tid) {tid_ = tid;}
    CountDownLatch& getCountDownLatch() {
        return latch_;
    }
private:
    bool start_;
    bool join_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    string name_;
    CountDownLatch latch_;
    static AtomicInt32 numCreated_;
};



inline int Thread::getNumCreated() {
    return numCreated_.get();
}



#endif // MUDUO_BASE_THREAD_H
