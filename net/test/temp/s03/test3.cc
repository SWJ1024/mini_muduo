#include "../../../Channel.h"
#include "../../../EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>

EventLoop* g_loop;

void timeout(Timestamp t)
{
  printf("%lu Timeout!\n", t.getmicroSecond()/Timestamp::M);
  g_loop->quit();
}

int main()
{
  EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  //bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);
  printf("before loop\n");
  loop.loop();
  printf("after loop\n");
  channel.disableAll();
  channel.remove();
  ::close(timerfd);
  printf("before close\n");
}
