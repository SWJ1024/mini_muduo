#include "Acceptor.h"
#include "SocketOps.h"
#include "InetAddress.h"
#include <unistd.h>
#include <error.h>
#include <fcntl.h>

Acceptor::Acceptor(EventLoop *loop, const InetAddress& listenAddr, bool reuseport) 
	: loop_(loop),
	  acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
	  acceptChannel_(loop, acceptSocket_.fd()),
	  listening_(false),
	  idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));

}

Acceptor::~Acceptor() {
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback& cb) {
	newConnectionCallback_ = cb;
}


void Acceptor::listen() {
	loop_->assertInLoopThread();
	listening_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}



void Acceptor::handleRead() {
	loop_->assertInLoopThread();
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0) {
		if (newConnectionCallback_) newConnectionCallback_(connfd, peerAddr);
		else sockets::close(connfd);
	}
	else {
		printf("error in handleread in acceptor\n");
	}
}


