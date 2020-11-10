#include "InetAddress.h"
#include "SocketOps.h"
#include "Endian.h"
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>

using namespace sockets;

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6), "InetAddress is same size as sockaddr_in6");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");


InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6) {
	static_assert(offsetof(InetAddress, addr6_) == 0, "addr6_ offset 0");
	static_assert(offsetof(InetAddress, addr_) == 0, "addr_ offset 0");
	if (ipv6) {
		addr6_.sin6_family = AF_INET6;
		in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
		addr6_.sin6_addr = ip;
		addr6_.sin6_port = hostToNetwork16(port);
	}
	else {
		addr_.sin_family = AF_INET;
		in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
		addr_.sin_addr.s_addr = hostToNetwork32(ip);
		addr_.sin_port = hostToNetwork16(port);
	}
}


InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6) {
	if (ipv6) {
		//memset(&addr6_, 0, sizeof addr6_);
		fromIpPort(ip.c_str(), port, &addr6_);	
	}
	else {
		//memset(&addr_, 0, sizeof addr_);
		fromIpPort(ip.c_str(), port, &addr_);
	}
}

static __thread char t_resolveBuffer[64*1024];

bool InetAddress::resolve(StringArg hostname, InetAddress* out) {
	assert(out != NULL);
	struct hostent hent;
	struct hostent* he = NULL;
	int herrno = 0;

	int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);

	if (ret == 0 && he != NULL) {
		assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
		out->addr_.sin_addr = *reinterpret_cast<struct in_addr*> (he->h_addr);
		return true;
	}
	else {
		if (ret) {
			printf("error in resolve in socket.cc\n");
		}
		return false;
	}
}


string InetAddress::toIp() const {
	char buf[64] = "";
	::toIp(buf, sizeof buf, getSockAddr());
	return buf;
}

string InetAddress::toIpPort() const {
	char buf[64] = "";
	::toIpPort(buf, sizeof buf, getSockAddr());
	return buf;
}

uint16_t InetAddress::toPort() const {
	return hostToNetwork16(portNetEndian());
}
