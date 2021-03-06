#include "PollPoller.h"
#include "Channel.h"
#include <assert.h>
#include <errno.h>
#include <poll.h>


PollPoller::PollPoller(EventLoop* loop)
	: Poller(loop)
{
}


PollPoller::~PollPoller() = default;

Timestamp PollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
	int savedErrno = errno;
	Timestamp now(Timestamp::now());
	if (numEvents > 0) {
		fillActiveChannels(numEvents, activeChannels);
	}
	else if (numEvents == 0) {
	}
	else {
		if (savedErrno != EINTR) {
			errno = savedErrno;
		}
	}
	return now;
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
	for (auto pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd) {
		if (pfd->revents > 0) {
			--numEvents;
			auto it = channels_.find(pfd->fd);
			assert(it != channels_.end());
			Channel* channel = it->second;
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			// pfd->revents = 0;
			activeChannels->push_back(channel);
		}
	}
}

void PollPoller::updateChannel(Channel* channel) {
	Poller::assertInLoopThread();
	if (channel->index() < 0) {
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size())-1;
		channel->set_index(idx);
		channels_[pfd.fd] = channel;
	}
	else {
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);
		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if (channel->isNoneEvent()) {
			// ignore this pollfd
			pfd.fd = -channel->fd()-1;
		}
	}
}

void PollPoller::removeChannel(Channel* channel) {
	Poller::assertInLoopThread();
	assert(channels_.find(channel->fd()) != channels_.end());
	assert(channels_[channel->fd()] == channel);
	assert(channel->isNoneEvent());
	int idx = channel->index();
	assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
	const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
	assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
	size_t n = channels_.erase(channel->fd());
	assert(n == 1); (void)n;
	if (static_cast<size_t>(idx) == pollfds_.size()-1) {
		pollfds_.pop_back();
	}
	else {
		int channelAtEnd = pollfds_.back().fd;
		iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
		if (channelAtEnd < 0) {
			channelAtEnd = -channelAtEnd-1;
		}
		channels_[channelAtEnd]->set_index(idx);
		pollfds_.pop_back();
	}
}

