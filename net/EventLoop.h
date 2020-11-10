#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "TimerId.h"
#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include "../base/Timestamp.h"
#include <functional>
#include <vector>
#include <memory>
#include <atomic>
#include <boost/any.hpp>

class Channel;
class Poller;
class TimerQueue;



class EventLoop {
public:
	using Functor = std::function<void()>;
	using TimerCallback = std::function<void()>;
	EventLoop();
	~EventLoop();
	void loop();
	void quit();
	void wakeup();
	void handleRead();
	void updateChannel(Channel*);
	void removeChannel(Channel*);
	void assertInLoopThread();
	int64_t iteration() const {return iteration_;}

	Timestamp pollReturnTime() const {return pollReturnTime_;}

	bool isInLoopThread() const{return threadId_ == CurrentThread::tid();}
	bool hasChannel(Channel*);
	bool eventHandling() const {return eventHandling_;}

	void runInLoop(Functor cb);
	void queueInLoop(Functor cb);
	TimerId runAt(Timestamp, TimerCallback);
	TimerId runAfter(double, TimerCallback);
	TimerId runEvery(double, TimerCallback);
	void cancel(TimerId);

	size_t queueSize() const;
	void doPendingFunctors();
	
	static EventLoop* getEventLoopOfCurrentThread();


	void setContext(const boost::any& context) {context_ = context;}
	const boost::any& getContext() const {return context_;}
	boost::any* getMutableContext() {return &context_;}

	boost::any context_;
   


private:
	using ChannelList = std::vector<Channel*>;

	ChannelList activeChannels_;     //the channels which poller returns
	Channel *currentActiveChannel_;  //the channel which is handled now 
	bool looping_;
	bool eventHandling_;
	bool callingPendingFunctors_;
	std::atomic<bool> quit_;
	const pid_t threadId_;
	
	Timestamp pollReturnTime_;

	std::unique_ptr<Poller> poller_;
	std::unique_ptr<TimerQueue> timerQueue_;
	
	int64_t iteration_;
	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;
	mutable MutexLock mutex_;
	std::vector<Functor> pendingFunctors_;
};


#endif
