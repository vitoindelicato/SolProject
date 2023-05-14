#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "worker.h"
#include "../lib/tools.h"
#include "../lib/socket_utils.h"
#include <sys/socket.h>

#define SOCKNAME "farm.sck"


extern int q_size;
extern int n_threads;
extern int t_delay;

extern pthread_cond_t not_empty;
extern int queue_interrupt;

extern struct sockaddr_un client_addr;


void master_worker(int argc, char **argv, char *dir_name) {
    
/* Queue init */

    _queue *queue = Malloc(sizeof(_queue));
    queue->items = Malloc(q_size * sizeof(char *));

    for (int i = 0; i < q_size; i++) {
        queue->items[i] = NULL;
    }


    mtx_init(&queue->q_lock, NULL);
    queue->done = 0;
    queue->size = q_size;
    queue->front = 0;
    queue->rear = 0;

    pthread_t threadpool[n_threads];


    for (int i = 0; i < n_threads; i++) {
        create(&threadpool[i], NULL, worker_function, (void *) queue);
    }


    for (int i = optind; i < argc; i++) {
        if(isRegular(argv[i])){
            if(queue_interrupt == 2){
                break;
            }
            enqueue(queue, argv[i]);
        }
    }


    if (dir_name != NULL) {
        if (isDir(dir_name)) {
            if (queue_interrupt != 2) {
                explorer(dir_name, queue);
            }

            /* enqueue() is called recursively from explorer() */
        }
    }


    lock(&queue->q_lock);
    queue->done = 1;

    /* At this point i will not insert any file into the queue
     * I need to send a not_empty signal in order to free all threads that where waiting
     * in (isEmpty && done == 0*/

    cond_signal(&not_empty);
    unlock(&queue->q_lock);


    for (int i = 0; i < n_threads; i++) {
        join(threadpool[i], NULL);
    }

    /*Once all workers returned, I can send a message to the server and then close it*/
    int fd = client_connection(&client_addr);
    ssize_t sent_bytes = write(fd, "DONE", 4);

    if (sent_bytes == -1){
        perror("Error on writing");
        free(queue->items);
        free(queue);
        return;
    }
    close(fd);

    free(queue->items);
    free(queue);
}
