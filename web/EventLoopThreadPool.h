#ifndef MUDUO_WEB_EVENTLOOPTHREADPOOL_H
#define MUDUO_WEB_EVENTLOOPTHREADPOOL_H
#include <memory>
#include <string>
#include <vector>
#include <functional>
using std::string;

class EventLoop;
class EventLoopThread;


class EventLoopThreadPool {
public:
	using ThreadInitCallback = std::function<void(EventLoop*)>;

	EventLoopThreadPool(EventLoop*, const string&);
	//~EventLoopThreadPool() {}
	~EventLoopThreadPool();

	void setThreadNum(int number) {numThread_ = number;}
	void start(const ThreadInitCallback& cb = ThreadInitCallback());

	bool started() const {return started_;}
	const string& name() const {return name_;}

	EventLoop* getNextLoop();
	EventLoop* getLoopForHash(size_t hashCode);
	std::vector<EventLoop*> getAllLoops();

private:
	EventLoop *baseLoop_;
	string name_;
	bool started_;
	int numThread_;
	int next_;

	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};


#endif
