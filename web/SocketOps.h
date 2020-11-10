#ifndef MUDUO_NET_SOCKETSOP_H
#define MUDUO_NET_SOCKETSOP_H

#include <arpa/inet.h>

namespace sockets {

int createNonblockingOrDie(sa_family_t family);
void shutdownWrite(int sockfd);

int  connect(int sockfd, const struct sockaddr* addr);
void bindOrDie(int sockfd, const struct sockaddr* addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in6* addr);
ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
ssize_t write(int sockfd, const void *buf, size_t count);
void close(int sockfd);


void toIpPort(char *buf, size_t size, const struct sockaddr* addr);
void toIp(char *buf, size_t size, const struct sockaddr* addr);

void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);


int getSocketError(int);


struct sockaddr_in6 getLocalAddr(int);
struct sockaddr_in6 getPeerAddr(int);
bool isSelfConnect(int);

}

#endif
