CC = gcc
CFLAGS = -Wall -g -pedantic -lpthread -lm
LIBS = ../lib

tools = $(LIBS)/tools*
queue = $(LIBS)/queue*
threads = worker*
system_calls = $(LIBS)/enhanced_sc*
collector = collector*
master_worker = master_worker*
node = $(LIBS)/node*

#$(LIBS)/tools.h $(LIBS)/queue.h $(LIBS)/tools.c $(LIBS)/queue.c
#$(tools) $(queue) $(threads)

.PHONY: default custom clean

farm	: farm.c $(master_worker) $(collector) $(tools) $(queue) $(threads) $(system_calls)  $(node)
	$(CC) $(CFLAGS) $^ -o $@


exe	:
	./main

custom :
	valgrind --track-origins=yes  --leak-check=full -s ./farm -q 3 -n 4 -d yeye file*


default :
	#rm farm.sck
	valgrind --track-origins=yes   --leak-check=full --show-leak-kinds=all -s ./farm file* -t 2000


clean :
	rm -r testdir
	rm file*