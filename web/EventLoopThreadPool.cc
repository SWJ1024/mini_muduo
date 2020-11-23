#include "EventLoopThreadPool.h"
#include "../net/EventLoop.h"
#include <assert.h>
#include "../net/EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg) 
	: baseLoop_(baseLoop),
	  name_(nameArg),
	  started_(false),
	  numThread_(0),
	  next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
	assert(!started_);
	baseLoop_->assertInLoopThread();
	started_ = true;
	for (int i = 0; i < numThread_; ++i) {
		char buf[name_.size() + 32];
		snprintf(buf, sizeof buf, "%s-%d", name_.c_str(), i);
		EventLoopThread* t = new EventLoopThread(cb, buf);
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		loops_.push_back(t->startLoop());
	}
	if (numThread_ == 0 && cb) cb(baseLoop_);
}



EventLoop* EventLoopThreadPool::getNextLoop() {
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop* loop = baseLoop_;
	if (!loops_.empty()) {
		loop = loops_[next_++];
		if (next_ >= static_cast<int>(loops_.size())) next_ = 0;
	}
	return loop;
}


EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode) {
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop* loop = baseLoop_;
	if (!loops_.empty()) {
		loop = loops_[hashCode %loops_.size()];
	}
	return loop;
}


std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
	baseLoop_->assertInLoopThread();
	assert(started_);
	if (loops_.empty()) {
		return std::vector<EventLoop*> (1, baseLoop_);
	}
	else return loops_;
}
