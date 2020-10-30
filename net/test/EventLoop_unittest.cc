#include "../EventLoop.h"
#include "../../base/Thread.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

//using namespace muduo;
//using namespace muduo::net;

EventLoop* g_loop;

void callback()
{
  printf("callback(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
  //EventLoop anotherLoop;
}

void threadFunc()
{
  printf("threadFunc(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
  printf("11\n");
  assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
  printf("22 %d\n", EventLoop::getEventLoopOfCurrentThread() == NULL ? 0 : 1);
  EventLoop loop;
  assert(EventLoop::getEventLoopOfCurrentThread() == &loop);
  printf("33 %d\n", EventLoop::getEventLoopOfCurrentThread() == NULL ? 0 : 1);
  loop.runAfter(3, callback);
  loop.loop();
}

int main()
{
  printf("main(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());

  printf("1\n");
  assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
  printf("1.5 %d\n", EventLoop::getEventLoopOfCurrentThread() == NULL ? 0 : 1);
  EventLoop loop;
  printf("2\n");
  assert(EventLoop::getEventLoopOfCurrentThread() == &loop);

  printf("2.5 %d\n", EventLoop::getEventLoopOfCurrentThread() == NULL ? 0 : 1);
  printf("3\n");
  Thread thread(threadFunc);
  printf("child\n");
  thread.start();
  loop.loop();
}
