#include "TcpConnection.h"
#include "Socket.h"
#include "../net/Channel.h"
#include <iostream>
#include "InetAddress.h"
#include "SocketOps.h"
#include "WeakCallback.h"


const char* TcpConnection::stateToString() const {
	switch (state_) {
		case kDisconnected:
			return "kDisconnected";
		case kConnecting:
			return "kConnecting";
		case kConnected:
			return "kConnected";
		case kDisconnecting:
			return "kDisconnecting";
		default:
			return "unknown state";
	}
}

bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const {
	return socket_->getTcpInfo(tcpi);
}

string TcpConnection::getTcpInfoString() const {
	char buf[1024];
	buf[0] = '\0';
	socket_->getTcpInfoString(buf, sizeof buf);
	return buf;
}



TcpConnection::TcpConnection(EventLoop *loop, const string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr) 
	: loop_(loop),
	name_(nameArg),
	state_(kConnecting),
	reading_(true),
	socket_(new Socket(sockfd)),
	channel_(new Channel(loop, sockfd)),
	localAddr_(localAddr),
	peerAddr_(peerAddr),
	highWaterMark_(64*1024*1024)
{
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
	socket_->setKeepAlive(true);
	std::cout << "TcpConnection::ctor[" <<  name_ << "] at " << this << " fd=" << sockfd << "\n";
}

TcpConnection::~TcpConnection() {
	std::cout << "TcpConnection::dtor[" <<  name_ << "] at " << this << " fd=" << channel_->fd() << " state=" << stateToString() << "\n";
	assert(state_ == kDisconnected);
}


void TcpConnection::handleRead(Timestamp receiveTime) {
	loop_->assertInLoopThread();
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	if (n > 0) {
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	}
	else if (n == 0) {
		handleClose();
	}
	else {
		errno = savedErrno;
		printf("TcpConnection::handleRead error\n");
		handleError();
	}
}

void TcpConnection::handleWrite() {
	loop_->assertInLoopThread();
	if (channel_->isWriting()) {
		ssize_t n = sockets::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
		if (n > 0) {
			outputBuffer_.retrieve(n);
			if (outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();
				if (writeCompleteCallback_) {
					loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
				}
				if (state_ == kDisconnecting) {
					shutDownInLoop();
				}
			}
		}
		else {
			printf("TcpConnection::handlewrite error\n");
		}
	}
	else {
		std::cout << "Connection fd = " << channel_->fd() << " is down, no more writing\n";
	}
}

void TcpConnection::handleClose() {
	loop_->assertInLoopThread();
	std::cout << "fd = " << channel_->fd() << " state = " << stateToString() << "\n";
	assert(state_ == kConnected || state_ == kDisconnecting);
	setState(kDisconnected);
	channel_->disableAll();

	TcpConnectionPtr guardThis(shared_from_this());
	connectionCallback_(guardThis);
	closeCallback_(guardThis);
}

void TcpConnection::handleError() {
	int err = sockets::getSocketError(channel_->fd());
	std::cout << "TcpConnection::handleError [" << name_ << "] - SO_ERROR = " << err << " " << strerror(err) << "\n";
}

void TcpConnection::connectEstablished() {
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->tie(shared_from_this());
	channel_->enableReading();
	connectionCallback_(shared_from_this());
}


void TcpConnection::connectDestroyed() {
	loop_->assertInLoopThread();
	if (state_ == kConnected) {
		setState(kDisconnected);
		channel_->disableAll();
		connectionCallback_(shared_from_this());
	}
	channel_->remove();
}

void TcpConnection::shutDown() {
	if (state_ == kConnected) {
		setState(kDisconnecting);
		loop_->runInLoop(std::bind(&TcpConnection::shutDownInLoop, this));
	}
}


void TcpConnection::shutDownInLoop() {
	loop_->assertInLoopThread();
	if (!channel_->isWriting()) {
		socket_->shutdownWrite();
	}
}




void TcpConnection::send(const void* data, int len) {
	send(StringPiece(static_cast<const char*>(data), len));
}


void TcpConnection::send(const StringPiece& message) {
	if (state_ == kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(message);
		}
		else {
			void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
			loop_->runInLoop(std::bind(fp, this, message.as_string()));
		}
	}
}


void TcpConnection::send(Buffer* buf) {
	if (state_ == kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(buf->peek(), buf->readableBytes());
			buf->retrieveAll();
		}
		else {
			void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
			loop_->runInLoop(std::bind(fp, this, buf->retrieveAllAsString()));
		}
	}
}


void TcpConnection::sendInLoop(const StringPiece& message) {
	sendInLoop(message.data(), message.size());
}


void TcpConnection::sendInLoop(const void* data, size_t len) {
	loop_->assertInLoopThread();
	ssize_t nwrote = 0, remaining = len;
	bool faultError = false;
	if (state_ == kDisconnected) {
		std::cout << "disconnected, give up writing\n";
		return;
	}
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		nwrote = sockets::write(channel_->fd(), data, len);
		if (nwrote >= 0) {
			remaining = len - nwrote;
			if (remaining == 0 && writeCompleteCallback_) {
				loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
			}
		}
		else {
			nwrote = 0;
			if (errno != EWOULDBLOCK) {
				std::cout << "TcpConnection::sendInLoop error\n";
				if (errno == EPIPE || errno == ECONNRESET) {
					faultError = true;
				}
			}
		}
	}

	assert(remaining <= len);
	if (!faultError && remaining > 0) {
		size_t oldLen = outputBuffer_.readableBytes();
		if (oldLen + remaining >= highWaterMark_
				&& oldLen < highWaterMark_
				&& highWaterMarkCallback_)
		{
			loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
		}
		outputBuffer_.append(static_cast<const char*>(data)+nwrote, remaining);
		if (!channel_->isWriting()) {
			channel_->enableWriting();
		}
	}
}



void TcpConnection::forceClose() {
	if (state_ == kConnected || state_ == kDisconnecting) {
		setState(kDisconnecting);
		loop_->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
	}
}

void TcpConnection::forceCloseWithDelay(double seconds) {
	if (state_ == kConnected || state_ == kDisconnecting) {
		setState(kDisconnecting);
		loop_->runAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::forceClose));
	}
}

void TcpConnection::forceCloseInLoop() {
	loop_->assertInLoopThread();
	if (state_ == kConnected || state_ == kDisconnecting) {
		handleClose();
	}
}

void TcpConnection::setTcpNoDelay(bool on)
{
	socket_->setTcpNoDelay(on);
}

void TcpConnection::startRead() {
	loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop() {
	loop_->assertInLoopThread();
	if (!reading_ || !channel_->isReading()) {
		channel_->enableReading();
		reading_ = true;
	}
}

void TcpConnection::stopRead() {
	loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop() {
	loop_->assertInLoopThread();
	if (reading_ || channel_->isReading()) {
		channel_->disableReading();
		reading_ = false;
	}
}
