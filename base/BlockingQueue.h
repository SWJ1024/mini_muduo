#ifndef MUDUO_BASE_BLOCKINGQUEUE_H
#define MUDUO_BASE_BLOCKINGQUEUE_H

#include <utility>
#include <deque>
#include "Condition.h"
#include "Mutex.h"
#include <assert.h>

template<typename T>
class BlockingQueue {
public:
	BlockingQueue() 
		: mutex_(),
		  notEmpty_(mutex_),
		  deq_()
	{
	}

	void put(const T& x) {
		MutexLockGuard lock(mutex_);
		deq_.push_back(x);
		notEmpty_.notify();
	}
	
	void put(T&& x) {
		MutexLockGuard lock(mutex_);
		deq_.push_back(std::move(x));
		notEmpty_.notify();
	}

	T take() {
		MutexLockGuard lock(mutex_);
		while (deq_.empty()) {
			notEmpty_.wait();
		}
		assert(!deq_.empty());
		T t(std::move(deq_.front()));
		deq_.pop_front();
		return t;
	}

	size_t size() {
		MutexLockGuard lock(mutex_);
		return deq_.size();
	}

private:
	MutexLock mutex_;
	Condition notEmpty_;
	std::deque<T> deq_;
};

#endif
