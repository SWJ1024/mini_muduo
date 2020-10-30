objects := $(wildcard ../../../*.cc ../../../../base/*.cc)

SOURCE := $(wildcard *.cc)
TARGETS := $(patsubst %.cc, %, $(SOURCE))


all : test1 #test2


test1 : test1.cc $(objects)
	g++ test1.cc $(objects) -o test1.out -pthread -std=c++17

test2 : test2.cc $(objects)
	g++ test2.cc $(objects) -o test2.out -pthread -std=c++17

:PHONY: clean
clean:
	-rm *.out
