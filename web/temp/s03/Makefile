objects := $(wildcard ../../../*.cc ../../../../base/*.o)

all : test3 test4 test5 test6

test1 : test1.cc $(objects)
	g++ test1.cc $(objects) -o test1.out -pthread -std=c++17


test2 : test2.cc $(objects)
	g++ test2.cc $(objects) -o test2.out -pthread -std=c++17


test3 : test3.cc $(objects)
	g++ test3.cc $(objects) -o test3.out -pthread -std=c++17


test4 : test4.cc $(objects)
	g++ test4.cc $(objects) -o test4.out -pthread -std=c++17


test5 : test5.cc $(objects)
	g++ test5.cc $(objects) -o test5.out -pthread -std=c++17


test6 : test6.cc $(objects)
	g++ test6.cc $(objects) -o test6.out -pthread -std=c++17

:PHONY: clean
clean:
	-rm *.out
