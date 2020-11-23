#include "TcpClient.h"
#include "Connector.h"
#include "../net/EventLoop.h"
#include "SocketOps.h"

void removeConnection(EventLoop *loop, const TcpConnectionPtr& conn) {
	loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector) {
	(void)connector;
}


void defaultTcpClientConnectionCallback(const TcpConnectionPtr& conn) {
	std::cout << conn->localAddress().toIpPort() << " -> "
		      << conn->peerAddress().toIpPort() << " is "
			  << (conn->connected() ? "UP" : "DOWN") << "\n";
}

void defaultTcpClientMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp) {
	buf->retrieveAll();
}


TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg) 
	: loop_(loop),
	connector_(new Connector(loop, serverAddr)),
	name_(nameArg),
	connectionCallback_(defaultTcpClientConnectionCallback),
	messageCallback_(defaultTcpClientMessageCallback),
	retry_(false),
	connect_(true),
	nextConnId_(1)
{
	connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
	std::cout << "TcpClient::TcpClient[" << name_ << "] - connector " << connector_ << "\n";
}

TcpClient::~TcpClient() {
	std::cout << "TcpClient::~TcpClient[" << name_ << "] - connector " << "\n";
	TcpConnectionPtr conn;
	bool unique = false;
	{
		MutexLockGuard lock(mutex_);
		unique = connection_.unique();
		conn = connection_;
	}
	if (conn) {
		assert(loop_ == conn->getLoop());
		CloseCallback cb = std::bind(&::removeConnection, loop_, std::placeholders::_1);
		loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
		if (unique) {
			conn->forceClose();
		}
	}
	else {
		connector_->stop();
		loop_->runAfter(1, std::bind(&::removeConnector, connector_));
	}
}

void TcpClient::connect() {
	std::cout << "TcpClient::connect[" << name_ << "] - connecting to " << connector_->serverAddress().toIpPort() << "\n";
	connect_ = true;
	connector_->start();
}


void TcpClient::disconnect() {
	connect_ = false;
	{
		MutexLockGuard lock(mutex_);
		if (connection_) {
			connection_->shutDown();
		}
	}
}


void TcpClient::stop() {
	connect_ = false;
	connector_->stop();
}


void TcpClient::newConnection(int sockfd) {
	loop_->assertInLoopThread();
	InetAddress peerAddress(sockets::getPeerAddr(sockfd));
	InetAddress localAddress(sockets::getLocalAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof buf, ":%s#%d", peerAddress.toIpPort().c_str(), nextConnId_++);
	string connName = name_ + buf;
	TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddress, peerAddress));
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}
	conn->connectEstablished();
}


void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
	loop_->assertInLoopThread();
	assert(loop_ == conn->getLoop());
	{
		MutexLockGuard lock(mutex_);
		assert(connection_ == conn);
		connection_.reset();
	}

	loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
	if (retry_ && connect_) {
		std::cout << "TcpClient::connect[" << name_ << "] - Reconnecting to " << connector_->serverAddress().toIpPort() << "\n";
		connector_->restart();
	}
}
