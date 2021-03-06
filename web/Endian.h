#ifndef MUDUO_WEB_ENDIAN_h
#define MUDUO_WEB_ENDIAN_h

#include <arpa/inet.h>
#include <stdint.h>

inline uint64_t hostToNetwork64(uint64_t host64) {
	return htobe64(host64);
	//return htonll(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32) {
	//return htobe32(host32);
	return htonl(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16) {
	//return htobe16(host16);
	return htons(host16);
}

inline uint64_t networkToHost64(uint64_t net64) {
	return be64toh(net64);
	//return ntohll(net64);
}

inline uint32_t networkToHost32(uint32_t net32) {
	//return be32toh(net32);
	return ntohl(net32);
}

inline uint16_t networkToHost16(uint16_t net16) {
	//return be16toh(net16);
	return ntohs(net16);
}




#endif
