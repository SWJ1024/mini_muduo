#include "Poller.h"
#include "Channel.h"
#include "PollPoller.h"
#include "EventLoop.h"
#include "EPollPoller.h"

Poller::Poller(EventLoop* loop)
	  : ownerLoop_(loop)
{
}

Poller::~Poller() = default;


bool Poller::hasChannel(Channel* channel) const {
	assertInLoopThread();
	auto it = channels_.find(channel->fd());
	return it != channels_.end() && it->second == channel;
}



Poller* Poller::newDefaultPoller(EventLoop* loop) {
	if (1) return new EPollPoller(loop);
	else return new PollPoller(loop);
}
