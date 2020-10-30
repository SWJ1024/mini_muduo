#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include <functional>
#include <memory>
#include "EventLoop.h"

class Channel {
public:
	using EventCallback = std::function<void()>;
	using ReadEventCallback = std::function<void(Timestamp)>;

	Channel(EventLoop*, int);
	~Channel();

	void handleEvent(Timestamp);
	void handleEventWithGuard(Timestamp);

	void update();
	void remove();
	
	int fd() const {return fd_;}
	int events() const {return events_;}
	void set_revents(int revt) {revents_ = revt;} //used by poller

	void enableReading() {events_ |= kReadEvent; update();}
	void disableReading() {events_ &= ~kReadEvent; update();}
	void enableWriting() {events_ |= kWriteEvent; update();}
	void disableWriting() {events_ &= ~kWriteEvent; update();}
	void disableAll() {events_ = kNoneEvent; update();}

	bool isWriting() const {return events_ & kWriteEvent;}
	bool isReading() const {return events_ & kReadEvent;}
	bool isNoneEvent() const {return events_ == kNoneEvent;}

	void setReadCallback(ReadEventCallback cb) {
		readCallback_ = std::move(cb);
	}
	void setWriteCallback(EventCallback cb) {
		writeCallback_ = std::move(cb);
	}
	void setErrorCallback(EventCallback cb) {
		errorCallback_ = std::move(cb);
	}
	void setCloseCallback(EventCallback cb) {
		closeCallback_ = std::move(cb);
	}

	EventLoop *ownerLoop() {return loop_;}

	//used by poller
	int index() {return index_;}
	void set_index(int idx) {index_ = idx;}
	//....

	void tie(const std::shared_ptr<void>&);
	void doNotLogHup() {logHup_= false;}
private:	
	bool logHup_;
	std::weak_ptr<void> tie_;
	bool tied_;
	
	EventLoop* loop_;
	const int fd_;
	int events_;
	int revents_;  // the received types of epoll/poll
	int index_;    // used by poller

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;
	
	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback errorCallback_;
	EventCallback closeCallback_;
	
	bool eventHandling_;
	bool addedToLoop_;
};

#endif
