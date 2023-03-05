#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../lib/queue.h"
#include "../lib/enhanced_sc.h"

extern pthread_cond_t empty;
extern pthread_cond_t not_empty;
extern pthread_cond_t full;
extern pthread_mutex_t q_mtx;


void *worker_function(void *args){
    _queue *queue = (_queue *) args;

    char *filename;
    //cleanup(args, &q_mtx);
    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data somewhere...*/

    lock(&q_mtx);

    while(isEmpty(queue)){
        cond_wait(&not_empty, &q_mtx);
    }

    filename = dequeue(queue);
    unlock(&q_mtx);
    printf("Thread %lu is working on file %s\n", pthread_self(), filename);

    return NULL;
}