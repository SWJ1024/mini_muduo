#include "ThreadPool.h"





ThreadPool::ThreadPool(const std::string& name) 
	: mutex_(),
	  notEmpty_(mutex_),
	  notFull_(mutex_),
	  name_(name),
	  maxSize_(0),
	  running_(false)
{
}

ThreadPool::~ThreadPool() {
	if (running_) {
		stop();
	}
}

void ThreadPool::setMaxQueueSize(int maxsize) {
	maxSize_ = maxsize;
}

void ThreadPool::setThreadInitCallback(const Task &t) {
	threadInitCallback_ = t;
}

void ThreadPool::start(int number) {
	assert(threads_.empty());
	running_ = true;
	threads_.reserve(number);
	for (int i = 0; i < number; ++i) {
		char id[32];
		snprintf(id, sizeof id, "%d", i+1);
		threads_.emplace_back(new Thread(
					std::bind(&ThreadPool::runInThread, this), name_ + id));
		threads_[i]->start();
	}
	if (number == 0 && threadInitCallback_) {
		threadInitCallback_();
	}
}


void ThreadPool::stop() {
	{
		MutexLockGuard lock(mutex_);
		running_ = false;
		notEmpty_.notifyall();
		notFull_.notifyall();
	}
	for (auto &i : threads_) i->join();
}


const std::string& ThreadPool::getname() const {
	return name_;
}

size_t ThreadPool::getSize() const {
	MutexLockGuard lock(mutex_);
	return deq_.size();
}

void ThreadPool::run(Task task) {
	if (threads_.empty()) task();
	else {
		MutexLockGuard lock(mutex_);
		while (isFull() && running_) {
			notFull_.wait();
		}
		if (!running_) return;
		assert(!isFull());
		deq_.push_back(std::move(task));
		notEmpty_.notify();
	}
}

bool ThreadPool::isFull() const {
	if (mutex_.getholder() != CurrentThread::tid())
		printf("%d %d\n", mutex_.getholder(), CurrentThread::tid());
	mutex_.assertLocked();
	return maxSize_ > 0 && maxSize_ <= deq_.size();
}

void ThreadPool::runInThread() {
	if (threadInitCallback_) {
		threadInitCallback_();
	}
	while (running_) {
		Task t(take());
		if (t) t();
	}
}

ThreadPool::Task ThreadPool::take() {
	MutexLockGuard lock(mutex_);
	while (deq_.empty() && running_) notEmpty_.wait();
	Task task;
	if (!deq_.empty()) {
		task = deq_.front();
		deq_.pop_front();
		if (maxSize_ > 0) notFull_.notify();
	}
	return task;
}
