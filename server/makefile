CC = gcc
CFLAGS = -Wall -g -std=c99 -pedantic
LFLAGS = 

OBJS = main.o 

all: lib server 

server: main.c
	$(CC) $(CFLAGS) -o $@ $^ -L../lib -ldrop
lib:
	$(MAKE) -C ../lib

.PHONY : clean
clean :
	rm -f server 
