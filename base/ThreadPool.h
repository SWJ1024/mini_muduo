#ifndef MUDUO_BASE_THREADPOOL_H
#define MUDUO_BASE_THREADPOOL_H

#include "Thread.h"
#include <memory>
#include <functional>
#include "Condition.h"
#include <deque>
#include <vector>

class ThreadPool {
public:
	using Task = std::function<void()>;
	
	explicit ThreadPool(const std::string& name = "ThreadPool");
	~ThreadPool();

	void setMaxQueueSize(int maxsize);
	void setThreadInitCallback(const Task &t);
	void start(int number);
	void stop();
	const std::string& getname() const;
	size_t getSize() const;
	void run(Task t);

	bool isFull() const;
	void runInThread();
	Task take();

private:
	mutable MutexLock mutex_;
	Condition notEmpty_, notFull_;
	std::string name_;
	Task threadInitCallback_;
	std::vector<std::unique_ptr<Thread>> threads_;
	std::deque<Task> deq_;
	size_t maxSize_;
	bool running_;
};




#endif
