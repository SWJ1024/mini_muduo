#include "SocketOps.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include <assert.h>
#include "Endian.h"

int sockets::createNonblockingOrDie(sa_family_t family) {
	int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (sockfd < 0) {
		printf("error in createNonblockingOrDie in socketops\n");
	}
	return sockfd;
}


int sockets::connect(int sockfd, const struct sockaddr* addr) {
	return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}


void sockets::bindOrDie(int sockfd, const struct sockaddr* addr) {
	int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof (struct sockaddr_in6)));
	if (ret < 0) {
		printf("error in bindordie socketops\n");
	} 
}


void sockets::listenOrDie(int sockfd) {
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0) {
		printf("error in listenordie socketops\n");
	}
}


int sockets::accept(int sockfd, struct sockaddr_in6* addr) {
	socklen_t addrlen = static_cast<socklen_t> (sizeof (*addr));
	int connfd = ::accept4(sockfd, reinterpret_cast<struct sockaddr*>(addr),
			&addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (connfd < 0)
		printf("error in accept socketops\n");
	return connfd;
}


ssize_t sockets::read(int sockfd, void *buf, size_t count) {
	return ::read(sockfd, buf, count);
}


ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt) {
	return ::readv(sockfd, iov, iovcnt);
}


ssize_t sockets::write(int sockfd, const void *buf, size_t count) {
	return ::write(sockfd, buf, count);
}


void sockets::close(int sockfd) {
	if (::close(sockfd) < 0) {
		printf("error in close socketops\n");
	}
}


void sockets::shutdownWrite(int sockfd) {
	if (::shutdown(sockfd, SHUT_WR) < 0) {
		printf("error in shutdown socketops\n");
	}
}


void sockets::toIpPort(char* buf, size_t size, const struct sockaddr* addr) {
	toIp(buf,size, addr);
	size_t end = strlen(buf);
	const struct sockaddr_in* addr4 = reinterpret_cast<const struct sockaddr_in*>(addr);
	uint16_t port = networkToHost16(addr4->sin_port);
	assert(size > end);
	snprintf(buf+end, size-end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size, const struct sockaddr* addr) {
	if (addr->sa_family == AF_INET) {
		assert(size >= INET_ADDRSTRLEN);
		const struct sockaddr_in* addr4 = reinterpret_cast<const struct sockaddr_in*>(addr);
		::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
	}
	else if (addr->sa_family == AF_INET6) {
		assert(size >= INET6_ADDRSTRLEN);
		const struct sockaddr_in6* addr6 = reinterpret_cast<const struct sockaddr_in6*>(addr);
		::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
	}
}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr) {
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
		printf("error in sockets::fromIpPort\n");
	}
}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr) {
	addr->sin6_family = AF_INET6;
	addr->sin6_port = hostToNetwork16(port);
	if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0) {
		printf("error in sockets::fromIpPort\n");
	}
}

int sockets::getSocketError(int sockfd) {
	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);

	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		return errno;
	}
	else {
		return optval;
	}
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd) {
	struct sockaddr_in6 localaddr;
	socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
	if (::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localaddr), &addrlen) < 0) {
		printf("error sockets::getLocalAddr\n");
	}
	return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd) {
	struct sockaddr_in6 peeraddr;
	socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
	if (::getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&peeraddr), &addrlen) < 0) {
		printf("error sockets::getPeerAddr\n");
	}
	return peeraddr;
}

bool sockets::isSelfConnect(int sockfd)
{
	struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
	struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
	if (localaddr.sin6_family == AF_INET) {
		const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
		const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
		return laddr4->sin_port == raddr4->sin_port
			&& laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
	}
	else if (localaddr.sin6_family == AF_INET6) {
		return localaddr.sin6_port == peeraddr.sin6_port
			&& memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
	}
	else {
		return false;
	}
}
