#include "TcpServer.h"
#include "../net/EventLoop.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include <iostream>
#include "TcpConnection.h"
#include "SocketOps.h"
#include "Buffer.h"

void defaultConnectionCallback(const TcpConnectionPtr& conn) {
	std::cout << conn->localAddress().toIpPort() << " -> "
		      << conn->peerAddress().toIpPort() << " is "
			  << (conn->connected() ? "UP" : "DOWN") << "\n";
}

void defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp) {
	buf->retrieveAll();
}


TcpServer::TcpServer(EventLoop *loop, const InetAddress& listenAddr, const string& nameArg, Option option) 
	: loop_(loop),
	name_(nameArg),
	ipPort_(listenAddr.toIpPort()),
	acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
	threadPool_(new EventLoopThreadPool(loop, name_)),
	connectionCallback_(defaultConnectionCallback),	
	messageCallback_(defaultMessageCallback),
	nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}


TcpServer::~TcpServer() {
	loop_->assertInLoopThread();
	for (auto &item : connections_) {
		TcpConnectionPtr conn(item.second);
		//channel不提升？？
		item.second.reset();
		conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
	}
}


void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
	loop_->assertInLoopThread();
	EventLoop *ioLoop = threadPool_->getNextLoop();
	char buf[64];
	snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_++);
	string connName = name_ + buf;
	std::cout << "TcpServer::newConnection [" << name_ << "] - new connection [" << connName << "] from " << peerAddr.toIpPort() << "\n";
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(ioLoop,
											connName,
											sockfd,
											localAddr,
											peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}



void TcpServer::start() {
	if (started_.getAndSet(1) == 0) {
		threadPool_->start(threadInitCallback_);
		assert(!acceptor_->listening());
		loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
	}
}


void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}


void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
	loop_->assertInLoopThread();
	std::cout << "TcpServer::removeConnectionInLoop [" << name_ << "] - connection " << conn->name() << "\n";
	size_t n = connections_.erase(conn->name());
	assert(n == 1); (void)n;
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}


void TcpServer::setThreadNum(int numThread) {
	assert(numThread >= 0);
	threadPool_->setThreadNum(numThread);
}
