
#include "../EventLoop.h"
#include "../../web/TcpClient.h"


TcpClient* g_client;

void timeout()
{
	std::cout << "timeout\n";
  g_client->stop();
}

int main()
{
  EventLoop loop;
  InetAddress serverAddr("127.0.0.1", 2); // no such server
  TcpClient client(&loop, serverAddr, "TcpClient");
  g_client = &client;
  loop.runAfter(0.0, timeout);
  loop.runAfter(1.0, std::bind(&EventLoop::quit, &loop));
  client.connect();
  CurrentThread::sleepUsec(100 * 1000);
  loop.loop();
}
