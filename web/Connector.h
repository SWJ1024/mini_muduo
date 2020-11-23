#ifndef MUDUO_WEB_CONNECTOR_H
#define MUDUO_WEB_CONNECTOR_H

#include "InetAddress.h"
#include <functional>
#include <memory>

class Channel;
class EventLoop;

class Connector : public std::enable_shared_from_this<Connector> {
public:
	using NewConnectionCallback = std::function<void(int)>;
	static const int kMaxRetryDelayMs = 30*1000;
	static const int kInitRetryDelayMs = 500;

	enum State{kDisconnected, kConnecting, kConnected};

	Connector(EventLoop*, const InetAddress&);
	~Connector();

	void setNewConnectionCallback(const NewConnectionCallback& cb) {newConnectionCallback_ = cb;}
	void setState(State s) {state_ = s;}
	const InetAddress& serverAddress() const {return serverAddr_;}

	void start();
	void startInLoop();
	void stop();
	void stopInLoop();

	void restart();
	void retry(int);
	void resetChannel();
	int removeAndResetChannel();

	void connect();
	void connecting(int);

	void handleWrite();
	void handleError();

private:
	EventLoop *loop_;
	InetAddress serverAddr_;
	bool connect_;
	State state_;
	int retryDelayMs_;

	std::unique_ptr<Channel> channel_;
	NewConnectionCallback newConnectionCallback_;
};

#endif 
