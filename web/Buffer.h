#ifndef MUDUO_WEB_BUFFER_H
#define MUDUO_WEB_BUFFER_H

#include <algorithm>
#include "SocketOps.h"
#include <vector>
#include <stdio.h>
#include <assert.h>
#include "StringPiece.h"
#include "Endian.h"

class Buffer {
public:
	static const size_t kCheapPrepand = 8;
	static const size_t kInitialSize = 1024; 

	explicit Buffer(size_t initialSize = kInitialSize)
		: buffer_(kCheapPrepand+initialSize),
		readerIndex_(kCheapPrepand),
		writerIndex_(kCheapPrepand)
	{
		assert(readableBytes() == 0);
		assert(writeableBytes() == initialSize);
		assert(prepandableBytes() == kCheapPrepand);
	}


	size_t readableBytes() const {return writerIndex_ - readerIndex_;}
	size_t writeableBytes() const {return buffer_.size() - writerIndex_;}
	size_t prepandableBytes() const {return readerIndex_;}



	void append(const char* data, size_t len) {
		ensureWriteableBytes(len);
		std::copy(data, data+len, beginWrite());
		hasWritten(len);
	}
	void append(const StringPiece& str) {
		append(str.data(), str.size());
	}
	void append(const void* data, size_t len) {
		append(static_cast<const char*>(data), len);
	}
	void appendInt64(int64_t x) {
		int64_t be64 = hostToNetwork64(x);
		append(&be64, sizeof be64);
	}
	void appendInt32(int32_t x) {
		int32_t be32 = hostToNetwork32(x);
		append(&be32, sizeof be32);
	}
	void appendInt16(int16_t x) {
		int16_t be16 = hostToNetwork16(x);
		append(&be16, sizeof be16);
	}
	void appendInt8(int8_t x) {
		append(&x, sizeof x);
	}



	void retrieveInt64() {retrieve(sizeof(int64_t));}
	void retrieveInt32() {retrieve(sizeof(int32_t));}
	void retrieveInt16() {retrieve(sizeof(int16_t));}
	void retrieveInt8()  {retrieve(sizeof(int8_t));}
	
	void retrieve(size_t len) {
		assert(len <= readableBytes());
		if (len < readableBytes()) {
			readerIndex_ += len;
		}
		else retrieveAll();
	}
	void retrieveUntil(const char* end) {
		assert(peek() <= end);
		assert(end <= beginWrite());
		retrieve(end - peek());
	}
	void retrieveAll() {
		readerIndex_ = writerIndex_ = kCheapPrepand;
	}
	string retrieveAsString(size_t len) {
		assert(len <= readableBytes());
		string res(peek(), len);
		retrieve(len);
		return res;
	}
	string retrieveAllAsString() {
		return retrieveAsString(readableBytes());
	}




	int64_t readInt64() {
		int64_t res = peekInt64();
		retrieveInt64();
		return res;
	}
	int32_t readInt32() {
		int64_t res = peekInt32();
		retrieveInt32();
		return res;
	}
	int16_t readInt16() {
		int16_t res = peekInt16();
		retrieveInt16();
		return res;
	}
	int8_t readInt8() {
		int64_t res = peekInt8();
		retrieveInt8();
		return res;
	}



	int64_t peekInt64() const {
		assert(readableBytes() >= sizeof(int64_t));
		int64_t be64 = 0;
		::memcpy(&be64, peek(), sizeof be64);
		return networkToHost64(be64);
	}
	int32_t peekInt32() const {
		assert(readableBytes() >= sizeof(int32_t));
		int32_t be32 = 0;
		::memcpy(&be32, peek(), sizeof be32);
		return networkToHost32(be32);
	}
	int16_t peekInt16() const {
		assert(readableBytes() >= sizeof(int16_t));
		int16_t be16 = 0;
		::memcpy(&be16, peek(), sizeof be16);
		return networkToHost16(be16);
	}
	int64_t peekInt8() const {
		assert(readableBytes() >= sizeof(int8_t));
		int8_t x = *peek();
		return x;
	}



	void prependInt64(int64_t x) {
		int64_t be64 = hostToNetwork64(x);
		prepend(&be64, sizeof be64);
	}
	void prependInt32(int32_t x) {
		int32_t be32 = hostToNetwork32(x);
		prepend(&be32, sizeof be32);
	}
    void prependInt16(int16_t x) {
		int16_t be16 = hostToNetwork16(x);
		prepend(&be16, sizeof be16);
	}
	void prependInt8(int8_t x) {
		prepend(&x, sizeof x);
	}


	const char* findCRLF() const {
		const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
		return crlf == beginWrite() ? NULL : crlf; 
	}
	const char* findCRLF(const char* start) const {
		assert(peek() <= start);
		assert(start <= beginWrite());
		const char *crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
		return crlf == beginWrite() ? NULL : crlf; 
	}
	const char* findEOL() const {
		const void* eol = memchr(peek(), '\n', readableBytes());
		return static_cast<const char*>(eol);
	}
	const char* findEOL(const char* start) const {
		assert(peek() <= start);
		assert(start <= beginWrite());
		const void* eol = memchr(start, '\n', beginWrite()-start);
		return static_cast<const char*> (eol);
	}


	char* beginWrite() {
		return begin() + writerIndex_;
	}
	const char* beginWrite() const {
		return begin() + writerIndex_;
	}
	void hasWritten(size_t len) {
		assert(len <= writeableBytes());
		writerIndex_ += len;
	}
	void unwrite(size_t len) {
		assert(len <= readableBytes());
		writerIndex_ -= len;
	}

	const char* peek() const {return begin() + readerIndex_;}
	StringPiece toStringPiece() const {return StringPiece(peek(), static_cast<int>(readableBytes()));}
	size_t internalCapacity() const {return buffer_.capacity();}

	void prepend(const void *data, size_t len) {
		assert(len <= prependableBytes());
		readerIndex_ -= len;
		const char* d = static_cast<const char*> (data);
		std::copy(d, d+len, begin()+readerIndex_);
	}

	void swap(Buffer& rhs) {
		buffer_.swap(rhs.buffer_);
		std::swap(readerIndex_, rhs.readerIndex_);
		std::swap(writerIndex_, rhs.writerIndex_);
	}

	void shrink(size_t reserve) {
		Buffer other;
		other.ensureWriteableBytes(prepandableBytes() + reserve);
		other.append(toStringPiece());
		swap(other);
	}


	void ensureWriteableBytes(size_t len) {
		if (writeableBytes() < len) {
			makeSpace(len);
		}
		assert(writeableBytes() >= len);
	}
	
	void makeSpace(size_t len) {
		if (writeableBytes() + prepandableBytes() < len + kCheapPrepand) {
			buffer_.resize(writerIndex_ + len);
		}
		else {
			assert(kCheapPrepand < readerIndex_);
			size_t readable = readableBytes();
			std::copy(begin()+readerIndex_, begin()+writerIndex_ ,begin() + kCheapPrepand);
			readerIndex_ = kCheapPrepand;
			writerIndex_ = readerIndex_ + readable;
			assert(readable == readableBytes());
		}
	}

	ssize_t readFd(int, int*);
private:
	char* begin() {return &*buffer_.begin();}
	const char* begin() const {return &*buffer_.begin();}


	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;
	static const char kCRLF[];
};



#endif

