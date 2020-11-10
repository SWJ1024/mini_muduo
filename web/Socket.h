#ifndef MUDUO_WEB_SOCKET_H
#define MUDUO_WEB_SOCKET_H

struct tcp_info;
class InetAddress;

class Socket {
public:
	explicit Socket(int sockfd) : sockfd_(sockfd) {}
	~Socket();

	int fd() const {return sockfd_;}

	bool getTcpInfo(struct tcp_info*) const;
	bool getTcpInfoString(char* buf, int len) const;

	void listen();
	int accept(InetAddress*);
	void bindAddress(const InetAddress&);

	void shutdownWrite();
	void setTcpNoDelay(bool);
	void setReuseAddr(bool);
	void setReusePort(bool);
	void setKeepAlive(bool);
private:
	const int sockfd_;
};


#endif
