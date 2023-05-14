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
socket = $(LIBS)/socket*

# Do not pay attention to custom and default, they are just for local testing purposes

.PHONY: default custom clean unlink test default_d custom_d

farm	: farm.c $(master_worker) $(collector) $(tools) $(queue) $(threads) $(system_calls) $(node) $(socket)
	$(CC) $(CFLAGS) $^ -o $@

generafile : generafile.c
	$(CC) $(CFLAGS) $^ -o $@


custom :
	valgrind --track-origins=yes  --leak-check=full --show-leak-kinds=all -s ./farm -q 1 -n 2 file* -t 5000

custom_d :
	valgrind --track-origins=yes  --leak-check=full --show-leak-kinds=all -s ./farm -q 2 -n 4 file* -t 5000 -d testdir



default :
	valgrind --track-origins=yes   --leak-check=full --show-leak-kinds=all -s ./farm file*

default_d :
	valgrind --track-origins=yes   --leak-check=full --show-leak-kinds=all -s ./farm file* -t 5000 -d testdir

test:
	make
	./test.sh

clean :
	rm -r testdir
	rm file*
	rm expected.txt

unlink :
	rm *.sck