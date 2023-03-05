CC = gcc
CFLAGS = -Wall -g -pedantic -lpthread
LIBS = ../lib

tools = $(LIBS)/tools*
queue = $(LIBS)/queue*
threads = worker_function*
system_calls = $(LIBS)/enhanced_sc*

#$(LIBS)/tools.h $(LIBS)/queue.h $(LIBS)/tools.c $(LIBS)/queue.c
#$(tools) $(queue) $(threads)

main	: main.c $(tools) $(queue) $(threads) $(system_calls)
	$(CC) $(CFLAGS) $^ -o $@


exe	:
	./main