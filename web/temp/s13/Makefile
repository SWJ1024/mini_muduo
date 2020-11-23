objects := $(wildcard ../../*.o ../../../base/*.o ../../../net/*.o)

all : test9 test10 test7 test8

test7 : test7.cc $(objects)
	g++ test7.cc $(objects) -o test7.out -pthread -g -std=c++17

test8 : test8.cc $(objects)
	g++ test8.cc $(objects) -o test8.out -pthread -g -std=c++17


test9 : test9.cc $(objects)
	g++ test9.cc $(objects) -o test9.out -pthread -g -std=c++17

test10 : test10.cc $(objects)
	g++ test10.cc $(objects) -o test10.out -pthread -g -std=c++17

:PHONY: clean
clean:
	-rm *.out
