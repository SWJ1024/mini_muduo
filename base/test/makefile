objects := $(wildcard ../*.cc)
#target := ThreadPool_test.cc
target := Date_unittest.cc

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
