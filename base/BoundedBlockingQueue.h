#ifndef MUDUO_BASE_BOUNDEDBLOCKINGQUEUE_H
#define MUDUO_BASE_BOUNDEDBLOCKINGQUEUE_H



#include "boost/circular_buffer.hpp"
#include "Condition.h"
#include "Mutex.h"
#include <assert.h>

template<typename T>
class BoundedBlockingQueue {
public:
	explicit BoundedBlockingQueue(int maxSize) 
		: mutex_(),
		  notEmpty_(mutex_),
		  notFull_(mutex_),
		  queue_(maxSize)
	{
	}

	void put(const T& x) {
		MutexLockGuard lock(mutex_);
		while (queue_.full()) {
			notFull_.wait();
		}
		assert(!queue_.full());
		queue_.push_back(x);
		notEmpty_.notify();
	}

	void put(T&& x) {	
		MutexLockGuard lock(mutex_);
		while (queue_.full()) {
			notFull_.wait();
		}
		assert(!queue_.full());
		queue_.push_back(std::move(x));
		notEmpty_.notify();
	}

	T take() {
		MutexLockGuard lock(mutex_);
		while (queue_.empty()) {
			notEmpty_.wait();
		}
		assert(!queue_.empty());
		T t(std::move(queue_.front()));
		queue_.pop_front();
		notFull_.notify();
		return t;
	}


	bool empty() const {
		MutexLockGuard lock(mutex_);
		return queue_.empty();
	}

	bool full() const {
		MutexLockGuard lock(mutex_);
		return queue_.full();
	}


	size_t size() const {
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

	size_t capacity() const {
		MutexLockGuard lock(mutex_);
		return queue_.capacity();
	}
private:
	mutable MutexLock mutex_;
	Condition notEmpty_, notFull_;
	boost::circular_buffer<T> queue_;
};




#endif
