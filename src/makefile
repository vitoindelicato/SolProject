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

# Do not pay attention to custom and default, they are just for local testing purposes

.PHONY: default custom clean

farm	: farm.c $(master_worker) $(collector) $(tools) $(queue) $(threads) $(system_calls) $(node)
	$(CC) $(CFLAGS) $^ -o $@

generafile : generafile.c
	$(CC) $(CFLAGS) $^ -o $@

exe	:
	./main

custom :
	valgrind --track-origins=yes  --leak-check=full -s ./farm -q 3 -n 4 file*


default :
	valgrind --track-origins=yes   --leak-check=full --show-leak-kinds=all -s ./farm file* -t 5000

test:
	make
	./test.sh

clean :
	rm -r testdir
	rm file*
	rm expected.txt