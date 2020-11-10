#ifndef MUDUO_WEB_TCPSERVER_H
#define MUDUO_WEB_TCPSERVER_H
#include "InetAddress.h"
#include "../base/Atomic.h"
#include "../base/Timestamp.h"
#include "TcpConnection.h"
#include <functional>
#include <memory>
#include <map>

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*,Timestamp)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

class TcpServer {
public:
	using ThreadInitCallback = std::function<void(EventLoop*)>;
	enum Option {kNoReusePort, kReusePort};

	TcpServer(EventLoop *, const InetAddress&, const string&, Option = kNoReusePort);
	~TcpServer();

	const string& ipPort() const {return ipPort_;}
	const string& name() const {return name_;}
	EventLoop* getLoop() const {return loop_;}
	std::shared_ptr<EventLoopThreadPool> threadPool() {return threadPool_;}

	void newConnection(int, const InetAddress&);
	void start();

	void removeConnection(const TcpConnectionPtr&);
	void removeConnectionInLoop(const TcpConnectionPtr&);

	void setThreadNum(int);
	void setConnectionCallback(const ConnectionCallback &cb) {connectionCallback_ = cb;}
	void setMessageCallback(const MessageCallback &cb) {messageCallback_ = cb;}
	void setWriteCompleteCallback(const WriteCompleteCallback &cb) {writeCompleteCallback_ = cb;}
	void setthreadInitCallback(const ThreadInitCallback& cb) {threadInitCallback_ = cb;}
private:
	using ConnectionMap = std::map<string, TcpConnectionPtr>;

	EventLoop* loop_;
	const string name_;
	const string ipPort_;
	AtomicInt32 started_;

	std::unique_ptr<Acceptor> acceptor_;
	std::shared_ptr<EventLoopThreadPool> threadPool_;

	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	ThreadInitCallback threadInitCallback_;

	ConnectionMap connections_;
	int nextConnId_;
};

#endif
