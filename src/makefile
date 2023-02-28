CC = gcc
CFLAGS = -Wall -g -pedantic
LIBS = ../lib


main	: main.c $(LIBS)/tools.h $(LIBS)/tools.c
	$(CC) $(CFLAGS) $^ -o $@


exe	:
	./main