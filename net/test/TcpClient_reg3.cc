#include "../../net/EventLoopThread.h"
#include "../../web/TcpClient.h"


int main()
{
//  Logger::setLogLevel(Logger::DEBUG);

  EventLoopThread loopThread;
  {
  InetAddress serverAddr("127.0.0.1", 1234); // should succeed
  TcpClient client(loopThread.startLoop(), serverAddr, "TcpClient");
  client.connect();
  CurrentThread::sleepUsec(500 * 1000);  // wait for connect
  client.disconnect();
  }

  CurrentThread::sleepUsec(1000 * 1000);
}
