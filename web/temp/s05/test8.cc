#include "../../TcpServer.h"
#include "../../../net/EventLoop.h"
#include "../../InetAddress.h"
#include <stdio.h>
#include "../../TcpConnection.h"
#include "../../Socket.h"
#include "../../SocketOps.h"

void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toIpPort().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const TcpConnectionPtr& conn,
               Buffer* data,
               Timestamp len)
{
  printf("onMessage(): received time  %ld, from connection [%s]\n",
         len.getSecond(), conn->name().c_str());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  InetAddress listenAddr(9981);
  EventLoop loop;
  string name = "myServer";
  TcpServer server(&loop, listenAddr, name);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}
