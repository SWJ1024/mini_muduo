objects := $(wildcard ../*.o)

SOURCE := $(wildcard *.cc)
TARGETS := $(patsubst %.cc, %, $(SOURCE))


all: Atomic_unittest BlockingQueue_bench BlockingQueue_test BoundedBlockingQueue_test  Date_unittest Mutex_test SingletonThreadLocal_test Singleton_test ThreadLocalSingleton_test ThreadLocal_test ThreadPool_test Thread_test Timestamp_unittest

Atomic_unittest : Atomic_unittest.cc $(objects)
	g++ Atomic_unittest.cc $(objects) -o Atomic_unittest.out -pthread -std=c++14

BlockingQueue_bench : BlockingQueue_bench.cc $(objects)
	g++ BlockingQueue_bench.cc $(objects) -o BlockingQueue_bench.out -pthread -std=c++14

BlockingQueue_test : BlockingQueue_test.cc $(objects)
	g++ BlockingQueue_test.cc $(objects) -o BlockingQueue_test.out -pthread -std=c++14

BoundedBlockingQueue_test : BoundedBlockingQueue_test.cc $(objects)
	g++ BoundedBlockingQueue_test.cc $(objects) -o BoundedBlockingQueue_test.out -pthread -std=c++14

Date_unittest : Date_unittest.cc $(objects)
	g++ Date_unittest.cc $(objects) -o Date_unittest.out -pthread -std=c++14


Mutex_test : Mutex_test.cc $(objects)
	g++ Mutex_test.cc $(objects) -o Mutex_test.out -pthread -std=c++14

SingletonThreadLocal_test : SingletonThreadLocal_test.cc $(objects)
	g++ SingletonThreadLocal_test.cc $(objects) -o SingletonThreadLocal_test.out -pthread -std=c++14

Singleton_test : Singleton_test.cc $(objects)
	g++ Singleton_test.cc $(objects) -o Singleton_test.out -pthread -std=c++14

ThreadLocalSingleton_test : ThreadLocalSingleton_test.cc $(objects)
	g++ ThreadLocalSingleton_test.cc $(objects) -o ThreadLocalSingleton_test.out -pthread -std=c++14

ThreadLocal_test : ThreadLocal_test.cc $(objects)
	g++ ThreadLocal_test.cc $(objects) -o ThreadLocal_test.out -pthread -std=c++14

ThreadPool_test : ThreadPool_test.cc $(objects)
	g++ ThreadPool_test.cc $(objects) -o ThreadPool_test.out -pthread -std=c++14


Thread_test : Thread_test.cc $(objects)
	g++ Thread_test.cc $(objects) -o Thread_test.out -pthread -std=c++14

Timestamp_unittest : Timestamp_unittest.cc $(objects)
	g++ Timestamp_unittest.cc $(objects) -o Timestamp_unittest.out -pthread -std=c++14


:PHONY: clean
clean:
	-rm *.out
