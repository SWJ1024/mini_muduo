CC=g++

SRCS=$(wildcard *.cc)
																					
OBJS=$(SRCS:.cc=.o)
 
 
DEBUG = -D_MACRO
 
CFLAGS = -Wall -c	
 
TARGET = main
 
$(TARGET): $(OBJS)	
	@mkdir -p output/
	$(CC) -std=c++17 $(OBJS) -pthread -o output/$(TARGET)
		 
%.o: %.cpp
	$(CC) -std=c++17 $(DEBUG) $(CFLAGS) -pthread $< -o $@  -w
	 
.PHONY: clean
clean:

