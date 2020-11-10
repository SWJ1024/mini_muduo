#include "../../web/InetAddress.h"
#include "../../web/SocketOps.h"
#define BOOST_TEST_MODULE InetAddressTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
//#include <boost/test/unit_test.hpp>
#include <string.h>
#include <stdio.h>
using std::string;

int main() {
  InetAddress addr0(1234);
  printf("%s,%s\n",addr0.toIp().c_str(), string("0.0.0.0").c_str());
  printf("%s,%s\n",addr0.toIpPort().c_str(), string("0.0.0.0:1234").c_str());
  printf("%d,%d\n",addr0.toPort(), 1234);

  InetAddress addr1(4321, true);
  printf("%s,%s\n",addr1.toIp().c_str(), string("127.0.0.1").c_str());
  printf("%s,%s\n",addr1.toIpPort().c_str(), string("127.0.0.1:4321").c_str());
  printf("%d,%d\n",addr1.toPort(), 4321);

  InetAddress addr2("1.2.3.4", 8888);
  printf("%s,%s\n",addr2.toIp().c_str(), string("1.2.3.4").c_str());
  printf("%s,%s\n",addr2.toIpPort().c_str(), string("1.2.3.4:8888").c_str());
  printf("%d,%d\n",addr2.toPort(), 8888);

  InetAddress addr3("255.254.253.252", 65535);
  printf("%s,%s\n",addr3.toIp().c_str(), string("255.254.253.252").c_str());
  printf("%s,%s\n",addr3.toIpPort().c_str(), string("255.254.253.252:65535").c_str());
  printf("%d,%d\n",addr3.toPort(), 65535);




  InetAddress addr(80);
  if (InetAddress::resolve("google.com", &addr))
  {
    printf("google.com resolved to %s\n", addr.toIpPort().c_str());
  }
  else
  {
    printf("Unable to resolve google.com\n");
  }
}


