CC = gcc
CFLAGS = -Wall -g -std=c99
LFLAGS = -lrt 

OBJS = ./server/main.c 

all:$(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $@ -o $* $(LFLAGS)

.PHONY : clean
clean :
	rm -f five_philosophers.o five_philosophers
