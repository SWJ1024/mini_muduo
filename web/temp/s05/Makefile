objects := $(wildcard ../../*.cc ../../../base/*.cc ../../../net/*.cc)

all : test7 test8

test7 : test7.cc $(objects)
	g++ test7.cc $(objects) -o test7.out -pthread -g -std=c++17

test8 : test8.cc $(objects)
	g++ test8.cc $(objects) -o test8.out -pthread -g -std=c++17

:PHONY: clean
clean:
	-rm *.out
