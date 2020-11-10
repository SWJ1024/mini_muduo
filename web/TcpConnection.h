#ifndef MUDUO_WEB_TCPCONNECTION_H
#define MUDUO_WEB_TCPCONNECTION_H

#include <memory>
#include <functional>
#include "InetAddress.h"
#include "../base/Timestamp.h"
#include "Buffer.h"
#include <boost/any.hpp>
class Channel;
class EventLoop;
class Socket;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;


class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
public:
	enum StateE{kDisconnected, kConnecting, kConnected, kDisconnecting};
	
	bool getTcpInfo(struct tcp_info*) const;
	string getTcpInfoString() const;

	TcpConnection(EventLoop *, const string&, int, const InetAddress&, const InetAddress&);
	~TcpConnection();

	void setConnectionCallback(const ConnectionCallback& cb) {connectionCallback_ = cb;}
	void setMessageCallback(const MessageCallback& cb) {messageCallback_ = cb;}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {writeCompleteCallback_ = cb;}
	void setCloseCallback(const CloseCallback& cb) {closeCallback_ = cb;}
	void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) {highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark;}
	void setState(StateE s) {state_ = s;}

	EventLoop* getLoop() const {return loop_;}
	const string& name() const {return name_;}
	const InetAddress& localAddress() const {return localAddr_;}
	const InetAddress& peerAddress() const {return peerAddr_;}
	bool connected() const {return state_ == kConnected;}
	bool disconnected() const {return state_ == kDisconnected;}

	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();

	Buffer* inputBuffer() {return &inputBuffer_;}
	Buffer* outputBuffer() {return &outputBuffer_;}
	
	const char* stateToString() const;
//////////////////////////////////////////////////////////////////////
	void connectEstablished();
	void connectDestroyed();

	void shutDown();
	void shutDownInLoop();

	void forceClose();
	void forceCloseInLoop();
	void forceCloseWithDelay(double seconds);

	void send(const void* message, int len);
	void send(const StringPiece& message);
	void send(Buffer* message);
	void sendInLoop(const StringPiece& message);
	void sendInLoop(const void* message, size_t len);

	bool isReading() const {return reading_;}
	
	void setTcpNoDelay(bool on);
	
	void startRead();
    void stopRead();
	void startReadInLoop();
	void stopReadInLoop();

	void setContext(const boost::any& context) {context_ = context;}
	const boost::any& getContext() const {return context_;}
	boost::any* getMutableContext() {return &context_;}
////////////////////////////////////////////////////////////////////////
private:
	EventLoop *loop_;
	const string name_;
	StateE state_;
	bool reading_;

	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	
	const InetAddress localAddr_;
	const InetAddress peerAddr_;

	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	CloseCallback closeCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;

	size_t highWaterMark_;

	Buffer inputBuffer_;
	Buffer outputBuffer_;

	boost::any context_;
};



#endif
