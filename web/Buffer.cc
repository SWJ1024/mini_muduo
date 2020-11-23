#include "Buffer.h"
#include <sys/uio.h>
#include "SocketOps.h"

const char Buffer::kCRLF[] = "\r\n";
const size_t Buffer::kCheapPrepand;
const size_t Buffer::kInitialSize;

ssize_t Buffer::readFd(int fd, int* savedError) {
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writeable = writeableBytes();
	vec[0].iov_base = begin() + writerIndex_;
	vec[0].iov_len = writeable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;

	const int iovcnt = (writeable <sizeof extrabuf) ?2 :1;
	const ssize_t n = sockets::readv(fd, vec, iovcnt);
	if (n < 0) *savedError = errno;
	else if (static_cast<size_t> (n) <= writeable) {
		writerIndex_ += n;
	}
	else {
		writerIndex_ = buffer_.size();
		append(extrabuf, n-writeable);
	}
	return n;
}
