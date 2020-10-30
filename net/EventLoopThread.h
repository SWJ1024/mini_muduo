#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include "../base/Condition.h"
#include "../base/Thread.h"
#include "../base/Mutex.h"
#include <string>
#include <functional>

class EventLoop;

class EventLoopThread {
public:
	using ThreadInitCallback = std::function<void(EventLoop*)>;
	EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
			const string &name = string());
	~EventLoopThread();
	EventLoop* startLoop();
	void threadFunc();

private:
	EventLoop* loop_;
	bool exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
	ThreadInitCallback callback_;
};





#endif
