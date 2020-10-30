#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include "../base/Mutex.h"
#include "../base/Timestamp.h"
#include "Channel.h"
#include <set>

class EventLoop;
class Timer;
class TimerId;


class TimerQueue {
public:
	using TimerCallback = std::function<void()>;
	explicit TimerQueue(EventLoop*);
	~TimerQueue();

	TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
	void cancel(TimerId timerId);

private:
	using Entry = std::pair<Timestamp, Timer*>;
	using TimerList = std::set<Entry>;
	using ActiveTimer = std::pair<Timer*, int64_t>;
	using ActiveTimerSet = std::set<ActiveTimer>;

	void addTimerInLoop(Timer*);
	void cancelInLoop(TimerId);
	void handleRead();

	std::vector<Entry> getExpired(Timestamp);
	void reset(const std::vector<Entry>&, Timestamp);

	bool insert(Timer*);

	EventLoop* loop_;
	const int timerfd_;
	Channel timerfdChannel_;
	TimerList timers_;

	ActiveTimerSet activeTimers_;
	bool callingExpiredTimers_;
	ActiveTimerSet cancelingTimers_;
};



#endif
