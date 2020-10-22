#include "Thread.h"
#include <assert.h>
#include "CurrentThread.h"
#include <pthread.h>


AtomicInt32 Thread::numCreated_;

class MainThread{
public:
    MainThread() {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        //pthread_atfork
    }
};

MainThread init;

void* startThread(void *p) {
    Thread* t = static_cast<Thread*> (p);
    t->setTid(CurrentThread::tid());
    printf("%d----%d\n", CurrentThread::tid(), t->getTid());
    CurrentThread::t_threadName = t->getName().empty() ? "muduoThread" : t->getName().c_str();
    t->getCountDownLatch().countDown();    
    t->run();
    CurrentThread::t_threadName = "finished";
    return NULL;
}


Thread::Thread(ThreadFunc f, const string &n) 
    : start_(false),
      join_(false),
      pthreadId_(0),
      tid_(0),
      func_(std::move(f)),
      name_(n),
      latch_(1)
{
    setDefaultName();
}



Thread::~Thread() {
    if (isStart() && !join_) {
        printf("name : %s, tid = %d is dead\n", name_.c_str(), tid_);
        pthread_detach(pthreadId_);
    }
}


void Thread::start() {
    assert(!isStart());
    start_ = true;
    if (pthread_create(&pthreadId_, NULL, &startThread, this) != 0) {
        start_ = false;
        printf("create error in Thread.cc\n");
    }
    else {
        latch_.wait();
//        assert(tid_ > 0);
    }
}


int Thread::join() {
    assert(isStart());
    assert(!join_);
    join_ = true;
    printf("name : %s, tid = %d is dead\n", name_.c_str(), tid_);
    return pthread_join(pthreadId_, NULL);
}


void Thread::setDefaultName() {
    int num = numCreated_.incrementAndGet();
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
}
