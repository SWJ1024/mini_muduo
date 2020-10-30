#ifndef MUDUO_NET_POLLER_EPOLLPOLLER_H
#define MUDUO_NET_POLLER_EPOLLPOLLER_H

#include "Poller.h"

class EPollPoller : public Poller {
public:
	EPollPoller(EventLoop *loop);
	~EPollPoller() override;

	Timestamp poll(int, ChannelList*) override;
	void updateChannel(Channel*) override;
	void removeChannel(Channel*) override;

	void fillActiveChannels(int, ChannelList*) const;
	void update(int, Channel*);
private:
	using EventList = std::vector<struct epoll_event>;

	static const int KInitEventListSize = 10;
	static const char* operationToString(int op);

	int epollfd_;
	EventList events_;
};


#endif
