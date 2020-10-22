objects := $(wildcard ../../*.cc)
mainfile := $(wildcard *.cc)

all: $(objects) $(mainfile)
    g++ $(mainfile) $(objects) -o app -pthread -std=c++14

#.PHONY: clean
#clean:
#    rm app
