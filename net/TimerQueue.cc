#include "TimerQueue.h"
#include "Timer.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>

struct timespec howMuchTimeFromNow(Timestamp when) {
	int64_t microseconds = when.getmicroSecond() - Timestamp::now().getmicroSecond();
	if (microseconds < 100) microseconds = 100;
	struct timespec ts;
	ts.tv_sec = static_cast<time_t> (microseconds/Timestamp::M);
	ts.tv_nsec = static_cast<long> ((microseconds%Timestamp::M)*1000);
	return ts;
}


int createTimerfd() {
	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0) {
		printf("error in createTimerfd TimerQueue.h");
	}
	return timerfd;
}

void resetTimerfd(int timerfd, Timestamp expiration) {
	struct itimerspec newValue, oldValue;
	memset(&newValue, 0, sizeof newValue);
	memset(&oldValue, 0, sizeof oldValue);
	newValue.it_value = howMuchTimeFromNow(expiration);
	timerfd_settime(timerfd, 0, &newValue, &oldValue);
}

void readTimerfd(int timerfd, Timestamp now) {
	uint64_t howmany;
	ssize_t n = read(timerfd, &howmany, sizeof howmany);
	printf("TimerQueue::handleRead %lu at %s\n", howmany, now.toString().c_str());
	if (n != sizeof howmany) {
		printf("error in readTimerfd  TimerQueue.cc\n");
	}
}

TimerQueue::TimerQueue(EventLoop* loop) 
	: loop_(loop),
	  timerfd_(createTimerfd()),
	  timerfdChannel_(loop, timerfd_),
	  timers_(),
	  callingExpiredTimers_(false)
{
	timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}


TimerQueue::~TimerQueue() {
	timerfdChannel_.disableAll();
	timerfdChannel_.remove();
	close(timerfd_);
	for (const auto & timer : timers_) {
		delete timer.second;
	}
}


TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
	Timer* timer = new Timer(std::move(cb), when, interval);
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
}


void TimerQueue::cancel(TimerId timerId) {
	loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}


void TimerQueue::addTimerInLoop(Timer* timer) {
	loop_->assertInLoopThread();
	bool earliestChanged = insert(timer);
	if (earliestChanged) {
		resetTimerfd(timerfd_, timer->expiration());
	}
}


void TimerQueue::cancelInLoop(TimerId timerId) {
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	ActiveTimer timer(timerId.timer_, timerId.sequence_);
	auto it = activeTimers_.find(timer);
	if (it != activeTimers_.end()) {
		size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
		assert(n == 1); (void)n;
		delete it->first;
		activeTimers_.erase(it);
	}
	else if (callingExpiredTimers_) {
		cancelingTimers_.insert(timer);
	}
	assert(timers_.size() == activeTimers_.size());
}


void TimerQueue::handleRead() {
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd_, now);
	std::vector<Entry> expired = getExpired(now);
	callingExpiredTimers_ = true;

	cancelingTimers_.clear();
	for (const Entry& it : expired) {
		it.second->run();
	}
	callingExpiredTimers_ = false;
	reset(expired, now);
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
	assert(timers_.size() == activeTimers_.size());
	std::vector<Entry> expired;
	Entry sentry(now, reinterpret_cast<Timer*> (UINTPTR_MAX));
	auto it = timers_.lower_bound(sentry);
	assert(it == timers_.end() || now < it->first);
	std::copy(timers_.begin(), it, back_inserter(expired));
	timers_.erase(timers_.begin(), it);
//put all expired timers into vector<Entry> expired
	for (const auto& i : expired) {
		ActiveTimer timer(i.second, i.second->sequence());
		size_t n = activeTimers_.erase(timer);
		assert(n == 1); (void)n;
	}
	assert(timers_.size() == activeTimers_.size());
	return expired;
}


void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) {
	Timestamp nextExpire;
	for (const Entry &it : expired) {
		ActiveTimer timer(it.second, it.second->sequence());
		if (it.second->repeat() && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
			it.second->restart(now);
			insert(it.second);
		}
		else {
			delete it.second;
		}
	}
	if (!timers_.empty()) {
		nextExpire = timers_.begin()->second->expiration();
	}
	if (nextExpire.valid()) {
		resetTimerfd(timerfd_, nextExpire);
	}
}


bool TimerQueue::insert(Timer* timer) {
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	bool earliestChanged = false;
	Timestamp when = timer->expiration();
	auto it = timers_.begin();
	if (it == timers_.end() || when < it->first) earliestChanged = true;
	{
		auto res = timers_.insert(Entry(when, timer));
		assert(res.second); (void)res;
	}
	{
		auto res = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
		assert(res.second); (void)res;
	}
	assert(timers_.size() == activeTimers_.size());
	return earliestChanged;
}

