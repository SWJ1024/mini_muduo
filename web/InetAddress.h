#ifndef MUDUO_WEB_INNETADDRESS_H
#define MUDUO_WEB_INNETADDRESS_H

#include <assert.h>
#include <variant>
#include <netinet/in.h>
#include "StringPiece.h"

class InetAddress {
public:
	explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
	InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);
	
	explicit InetAddress(const struct sockaddr_in6& addr) : addr6_(addr) {}
	explicit InetAddress(const struct sockaddr_in& addr) : addr_(addr) {}

	static bool resolve(StringArg hostname, InetAddress * res);

	sa_family_t family() const {return addr_.sin_family;}
	string toIp() const;
	string toIpPort() const;
	uint16_t toPort() const;

	const struct sockaddr* getSockAddr() const {
		return reinterpret_cast<const struct sockaddr*>(&addr6_);
	} 
	void setSockAddrInet6(const struct sockaddr_in6 &addr6) {addr6_ = addr6;}
	
	uint32_t ipNetEndian() const {
		assert(family() == AF_INET);
		return addr_.sin_addr.s_addr;
	}

	uint16_t portNetEndian() const {
		return addr_.sin_port;
	}

	void setScopedId(uint32_t scope_id) {
		if (family() == AF_INET6) {
			addr6_.sin6_scope_id = scope_id;
		}
	}

private:
	union {
		struct sockaddr_in addr_;
		struct sockaddr_in6 addr6_;
	};
//	std::variant<struct sockaddr_in, struct sockaddr_in6> addr_;
};


#endif
