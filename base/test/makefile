objects := $(wildcard ../*.cc)
target := Thread_test.cc

ver = debug
ifeq ($(ver), debug)
all: $(target) $(objects)
	g++ $(objects) $(target) -D_DEBUG -o a.out -pthread -std=c++14
else
all: $(target) $(objects)
	g++ $(objects) $(target) -o a.out -pthread -std=c++14
endif

:PHONY: clean
clean:
	rm a.out
