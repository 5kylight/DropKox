CC = gcc
CFLAGS = -Wall -g -std=c99 -pedantic
LFLAGS = 

OBJS = main.o 

all: server 

server : main.c
	$(CC) $(CFLAGS) -o $@ $^ -L../lib -ldrop

.PHONY : clean
clean :
	rm -f server 
