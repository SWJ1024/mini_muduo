#ifndef MUDUO_WEB_TCPCLIENT_H
#define MUDUO_WEB_TCPCLIENT_H

#include "../base/Mutex.h"
#include <memory>
#include "TcpConnection.h"

class EventLoop;
class Connector;



typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;
using ConnectorPtr = std::shared_ptr<Connector>;

class TcpClient {
public:
	TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg);
	~TcpClient();

	void connect();
	void disconnect();
	void stop();


	const string& name() const { return name_; }
	EventLoop* getLoop() const {return loop_;}
	bool retry() const {return retry_;}

	void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = std::move(cb); }
	void setMessageCallback(MessageCallback cb){ messageCallback_ = std::move(cb); }
	void setWriteCompleteCallback(WriteCompleteCallback cb) { writeCompleteCallback_ = std::move(cb); }
	void enableRetry() {retry_ = true;}

	void newConnection(int sockfd);
	void removeConnection(const TcpConnectionPtr& conn);

	TcpConnectionPtr connection() const{
		MutexLockGuard lock(mutex_);
		return connection_;
	}

private:
	
	EventLoop* loop_;
	ConnectorPtr connector_;
	const string name_;

	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;

	bool retry_;
	bool connect_;
	int nextConnId_;

	mutable MutexLock mutex_;
	TcpConnectionPtr connection_;
};


#endif
