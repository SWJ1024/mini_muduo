#ifndef MUDUO_NET_TIMERID_H
#define MUDUO_NET_TIMERID_H

#include <stdio.h>
#include <stdint.h>

class Timer;

class TimerId{
friend class TimerQueue;
public:
	TimerId() 
		: timer_(NULL),
		  sequence_(0)
	{
	}

	TimerId(Timer* timer, int64_t seq) 
		: timer_(timer),
		  sequence_(seq)
	{
	}

private:
	Timer* timer_;
	int64_t sequence_;
};


#endif
