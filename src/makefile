CC = gcc
CFLAGS = -Wall -g -pedantic
LIBS = ../lib


main	: main.c $(LIBS)/tools.h $(LIBS)/queue.h $(LIBS)/tools.c $(LIBS)/queue.c
	$(CC) $(CFLAGS) $^ -o $@


exe	:
	./main