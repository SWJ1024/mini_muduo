objects := $(wildcard ../*.o ../../base/*.o ../../web/*.cc)

#SOURCE := $(wildcard *.cc)
#TARGETS := $(patsubst %.cc, %, $(SOURCE))


all : InetAddress_unittest TcpClient_reg1 TcpClient_reg2 TcpClient_reg3 EchoClient_unittest #EventLoop_unittest MyTimerTest03 MyTimerTest04 TimerQueue_unittest


EchoClient_unittest : EchoClient_unittest.cc $(objects)
	g++ EchoClient_unittest.cc $(objects) -o EchoClient_unittest.out -pthread -std=c++17

TcpClient_reg1 : TcpClient_reg1.cc $(objects)
	g++ TcpClient_reg1.cc $(objects) -o TcpClient_reg1.out -pthread -std=c++17


TcpClient_reg2 : TcpClient_reg2.cc $(objects)
	g++ TcpClient_reg2.cc $(objects) -o TcpClient_reg2.out -pthread -std=c++17

	
TcpClient_reg3 : TcpClient_reg3.cc $(objects)
	g++ TcpClient_reg3.cc $(objects) -o TcpClient_reg3.out -pthread -std=c++17

InetAddress_unittest : InetAddress_unittest.cc $(objects)
	g++ InetAddress_unittest.cc $(objects) -o InetAddress_unittest.out -pthread -std=c++17


EventLoop_unittest : EventLoop_unittest.cc $(objects)
	g++ EventLoop_unittest.cc $(objects) -o EventLoop_unittest.out -pthread -std=c++17


MyTimerTest03 : MyTimerTest03.cc $(objects)
	g++ MyTimerTest03.cc $(objects) -o MyTimerTest03.out -pthread -std=c++17

MyTimerTest04 : MyTimerTest04.cc $(objects)
	g++ MyTimerTest04.cc $(objects) -o MyTimerTest04.out -pthread -std=c++17

TimerQueue_unittest : TimerQueue_unittest.cc $(objects)
	g++ TimerQueue_unittest.cc $(objects) -o TimerQueue_unittest.out -pthread -std=c++17

:PHONY: clean
clean:
	-rm *.out
