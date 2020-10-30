#include "EPollPoller.h"
#include "Channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;


EPollPoller::EPollPoller(EventLoop *loop) 
	: Poller(loop),
	epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
	events_(KInitEventListSize)
{
	if (epollfd_ < 0) {
		printf("error in EPollPoller construction\n");
	}
}


EPollPoller::~EPollPoller() {
	close(epollfd_);	
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannel) {
	int numsEvent = epoll_wait(epollfd_, &*events_.begin(), static_cast<int> (events_.size()), timeoutMs);
	Timestamp now(Timestamp::now());
	if (numsEvent > 0) {
		fillActiveChannels(numsEvent, activeChannel);
		if (static_cast<size_t> (numsEvent) == events_.size()) {
			events_.resize(events_.size()*2);
		}
	}
	else if (numsEvent == 0) {
		printf("no thing happened\n");
	}
	else {
		printf("error in EpollPoller::poll\n");
	}
	return now;
}

void EPollPoller::updateChannel(Channel* channel) {
	Poller::assertInLoopThread();
	const int index = channel->index();
	int fd = channel->fd();
	if (index == kNew || index == kDeleted) {
		if (index == kNew) {
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		else {
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else {
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(index == kAdded);
		if (channel->isNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		else {
			update(EPOLL_CTL_MOD, channel);
		}
	}
}


void EPollPoller::update(int op, Channel* channel) {
	struct epoll_event event;
	memset(&event, 0, sizeof event);
	event.events = channel->events();
	event.data.ptr= channel;
	int fd = channel->fd();
	if (epoll_ctl(epollfd_, op, fd, &event) < 0) {
		if (op == EPOLL_CTL_DEL) {
			printf("error del in update EPollPoller.cc\n");
		}
		else {
			printf("error mod or add in update EPollPoller.cc\n");
		}
	}
}


void EPollPoller::removeChannel(Channel* channel) {
	Poller::assertInLoopThread();
	int fd = channel->fd();
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->isNoneEvent());
	int index = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n = channels_.erase(fd);
	assert(n == 1);  (void)n;
	if (index == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}


void EPollPoller::fillActiveChannels(int numsEvents, ChannelList* activeChannels) const {
	assert (static_cast<size_t> (numsEvents) <= events_.size());
	for (int i = 0; i < numsEvents; ++i) {
		Channel* channel = static_cast<Channel*> (events_[i].data.ptr);
		int fd = channel->fd();
		auto it = channels_.find(fd);
		assert(it != channels_.end());
		assert(it->second == channel);
		(void)it;
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}
