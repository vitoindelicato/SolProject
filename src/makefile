CC = gcc
CFLAGS = -Wall -g -pedantic -lpthread
LIBS = ../lib

tools = $(LIBS)/tools*
queue = $(LIBS)/queue*
threads = worker_function*
system_calls = $(LIBS)/enhanced_sc*

#$(LIBS)/tools.h $(LIBS)/queue.h $(LIBS)/tools.c $(LIBS)/queue.c
#$(tools) $(queue) $(threads)

.PHONY: default custom

main	: main.c $(tools) $(queue) $(threads) $(system_calls)
	$(CC) $(CFLAGS) $^ -o $@


exe	:
	./main


custom :
	valgrind --track-origins=yes ./main -q 3 -n 4 -d yeye file*


default :
	valgrind --track-origins=yes ./main -d yeye file*