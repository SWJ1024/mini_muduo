#ifndef MUDUO_WEB_ACCEPTOR_H
#define MUDUO_WEB_ACCEPTOR_H

#include "../net/Channel.h"
#include "../net/EventLoop.h"
#include "Socket.h"

class EventLoop;
class InetAddress;

class Acceptor {
public:
	using NewConnectionCallback =  std::function<void(int sockfd, const InetAddress&)>;
	Acceptor(EventLoop *, const InetAddress&, bool = true);
	~Acceptor();
	void setNewConnectionCallback(const NewConnectionCallback&);
	void listen();
	bool listening() const {return listening_;}

	void handleRead();
private:
	EventLoop * loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listening_;
	int idleFd_;
};



#endif
