#include "Connector.h"
#include "../net/EventLoop.h"
#include "SocketOps.h"
#include <stdio.h>
#include "../net/Channel.h"
#include <iostream>
using std::cout;


const int Connector::kMaxRetryDelayMs;
const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop *loop, const InetAddress& serverAddr) 
	: loop_(loop),
	serverAddr_(serverAddr),
	connect_(false),
	state_(kDisconnected),
	retryDelayMs_(kInitRetryDelayMs)
{
}


Connector::~Connector() {
	assert(!channel_); 
}



void Connector::start() {
	connect_ = true;
	loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}


void Connector::startInLoop() {
	loop_->assertInLoopThread();
	assert(state_ == kDisconnected);
	if (connect_) {
		connect();
	}
	else {
		printf("error do not connect in startInLoop\n");
	}
}

void Connector::restart() {
	loop_->assertInLoopThread();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	connect_ = true;
	startInLoop();
}


void Connector::retry(int sockfd) {
	sockets::close(sockfd);
	setState(kDisconnected);
	if (connect_) {
		std::cout << "Connector::retry - Retry connecting to " << serverAddr_.toIpPort() << " in " << retryDelayMs_ << " milliseconds.\n";
		loop_->runAfter(retryDelayMs_/1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
		retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
	}
	else {
		printf("do not connect printed in retry function\n");
	}
}


void Connector::stop() {
	connect_ = false;
	loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
}


void Connector::stopInLoop() {
	loop_->assertInLoopThread();
	if (state_ == kConnecting) {
		setState(kDisconnected);
		int sockfd = removeAndResetChannel();
		retry(sockfd);
	}
}



void Connector::connect() {
	int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
	int savedErrno = (ret == 0) ? 0 : errno;
	switch(savedErrno) {
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;

		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
			printf("connect error in Connector::startInLoop %d\n", savedErrno);
			sockets::close(sockfd);
			break;

		default:
			printf("Unexpected error in Connector::startInLoop %d\n", savedErrno);
			sockets::close(sockfd);
			break;
	}
}


void Connector::connecting(int sockfd) {
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
	channel_->setErrorCallback(std::bind(&Connector::handleError, this));
	channel_->enableWriting();
}



void Connector::handleWrite() {
	if (state_ == kConnecting) {
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if (err) {
			cout <<  "Connector::handleWrite - SO_ERROR = "
				 << err << " " << strerror(err) << "\n";
			retry(sockfd);
		}
		else if (sockets::isSelfConnect(sockfd)) {
			cout << "Connector::handleWrite - self connect\n";
			retry(sockfd);
		}
		else {
			setState(kConnected);
			if (connect_) newConnectionCallback_(sockfd);
			else sockets::close(sockfd);
		}
	}
	else {
		assert(state_ == kDisconnected);
	}
}


void Connector::handleError() {
	cout << "Connector::handleError state=" << state_ << "\n";
	if (state_ == kConnecting) {
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		std::cout << "so_error = " << err << strerror(err) << "\n";
		retry(sockfd);
	}
}


void Connector::resetChannel() {
	channel_.reset();
}


int Connector::removeAndResetChannel() {
	channel_->disableAll();
	channel_->remove();
	int sockfd = channel_->fd();
	loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
	return sockfd;
}
