#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H

#include "../base/Atomic.h"
#include "../base/Timestamp.h"
#include <functional>

class Timer {
public:
	using TimerCallback =  std::function<void()>;
	Timer(TimerCallback cb, Timestamp when, double interval) 
		: callback_(std::move(cb)),
		  expiration_(when),
		  interval_(interval),
		  repeat_(interval > 0),
		  sequence_(s_numCreated_.incrementAndGet())
	{
	}

	void run() const {callback_();}

	Timestamp expiration() const {return expiration_;}

	bool repeat() const {return repeat_;}

	int64_t sequence() const {return sequence_;}

	void restart(Timestamp);
	static int64_t numCreated() {return s_numCreated_.get();}
private:
	const TimerCallback callback_;
	Timestamp expiration_;
	const double interval_;
	const bool repeat_;
	const int64_t sequence_;

	static AtomicInt64 s_numCreated_;
};


#endif
