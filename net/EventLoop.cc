#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"
#include <sys/eventfd.h>

const int kPollTimeMs = 10000;

__thread EventLoop* t_loopInThisThread = NULL;

int createEventfd() {
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		printf("error in createEventfd   EventLoop\n");
	}
	return evtfd;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
	//	printf("getEventLoopOfCurrentThread %d\n", CurrentThread::tid());
	return t_loopInThisThread;
}

EventLoop::EventLoop()
  : currentActiveChannel_(NULL),
	looping_(false),
	eventHandling_(false),
	callingPendingFunctors_(false),
	quit_(false),
	threadId_(CurrentThread::tid()),
	poller_(Poller::newDefaultPoller(this)),
	timerQueue_(new TimerQueue(this)),
	//poller_(Poller::newDefaultPoller(this)), 
	iteration_(0),
	wakeupFd_(createEventfd()),
	wakeupChannel_(new Channel(this, wakeupFd_))
{
	//one thread one loop
	if (t_loopInThisThread) {
		printf("error in EventLoop() EventLoop.h threadId = %d\n", threadId_);
	}
	else {
		t_loopInThisThread = this;
	}
	wakeupChannel_->setReadCallback(
			std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}


EventLoop::~EventLoop() {
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	close(wakeupFd_);
	t_loopInThisThread = NULL;
}



size_t EventLoop::queueSize() const {
	MutexLockGuard lock(mutex_);
	return pendingFunctors_.size();
}


void EventLoop::doPendingFunctors() {
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for (const auto &f : functors) f();

	callingPendingFunctors_ = false;
}


//cannot be called by cross thread
void EventLoop::loop() {
	assert(!looping_);
	assertInLoopThread();
	quit_ = false;
	looping_ = true;
	while (!quit_) {
		activeChannels_.clear();
		pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
		++iteration_;
		eventHandling_ = true;
		for (auto *channel : activeChannels_) {
			currentActiveChannel_ = channel;
			currentActiveChannel_->handleEvent(pollReturnTime_);
		}
		currentActiveChannel_ = NULL;
		eventHandling_ = false;
		doPendingFunctors();
	}
	looping_ = false;
}

void EventLoop::quit() {
	quit_ = true;
	if (!isInLoopThread()) wakeup();
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		printf("error in wakeup EventLoop.cc\n");
	}
}


void EventLoop::handleRead() {
	uint64_t one = 1;
	ssize_t n = read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		printf("error in handleRead EventLoop.cc\n");
	}
}


bool EventLoop::hasChannel(Channel *channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	return poller_->hasChannel(channel);
}


void EventLoop::assertInLoopThread() {
	if (!isInLoopThread()) {
		printf("error assertInLoopThread\n");
		exit(1);
	}
}

void EventLoop::updateChannel(Channel *channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}


void EventLoop::queueInLoop(Functor cb) {
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(std::move(cb));
	}
	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();
	}
}


void EventLoop::removeChannel(Channel *channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	if (eventHandling()) {
			assert(currentActiveChannel_ == channel || 
				std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
	}
	poller_->removeChannel(channel);

}

void EventLoop::cancel(TimerId timerId) {
	return timerQueue_->cancel(timerId);
}


void EventLoop::runInLoop(Functor cb) {
	if (isInLoopThread()) cb();  //in loop, insert now
	else queueInLoop(std::move(cb));
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback cb) {
	return timerQueue_->addTimer(std::move(cb), time, 0.0);
}


TimerId EventLoop::runAfter(double delay, TimerCallback cb) {
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, std::move(cb));
}


TimerId EventLoop::runEvery(double interval, TimerCallback cb) {
	Timestamp time(addTime(Timestamp::now(), interval));
	return timerQueue_->addTimer(std::move(cb), time, interval);
}
