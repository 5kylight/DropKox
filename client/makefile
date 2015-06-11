C = gcc
CFLAGS = -Wall -g -std=c99 -pedantic
LIBS = ldrop

all: client  

client: main.c 
	$(CC) $(CFLAGS) -o $@ $^ -L../lib -ldrop
clean:
	rm client 
