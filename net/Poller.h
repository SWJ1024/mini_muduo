#ifndef MUDUO_NET_POLLER_H
#define MUDUO_NET_POLLER_H

#include <map>
#include <vector>
#include "EventLoop.h"

class Channel;

class Poller {
public:
	using ChannelList = std::vector<Channel*>;
	using ChannelMap = std::map<int, Channel*>;
	
	Poller(EventLoop* loop);
	virtual ~Poller();
		
	virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
	virtual void updateChannel(Channel* channel) = 0;
	virtual void removeChannel(Channel* channle) = 0;
	virtual bool hasChannel(Channel* channel) const;
	
	static Poller* newDefaultPoller(EventLoop*);

	void assertInLoopThread() const {
		ownerLoop_->assertInLoopThread();
	}

protected:
	ChannelMap channels_;
	EventLoop* ownerLoop_;
};


#endif
