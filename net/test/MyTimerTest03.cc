#include "../Channel.h"
#include "../EventLoop.h"
#include <cstring>
#include <stdio.h>
#include <sys/timerfd.h>
#include <stdlib.h>
#define bzero(a, b) memset(a, 0, b)
EventLoop* g_loop;
int timerfd;

void timeout(Timestamp t)
{
	printf("Timeout!\n");
	uint64_t howmany;
	::read(timerfd, &howmany, sizeof howmany);
	g_loop->quit();
}

int main()
{
	EventLoop loop;
	g_loop = &loop;

	timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(&loop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 5;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();

	::close(timerfd);
}
