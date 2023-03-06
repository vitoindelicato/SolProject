#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../lib/queue.h"
#include "../lib/enhanced_sc.h"

extern pthread_cond_t empty;
extern pthread_cond_t not_empty;
extern pthread_cond_t full;
extern pthread_mutex_t q_mtx;
extern int done;


void *worker_function(void *args){
    _queue *queue = (_queue *) args;

    char *filename;
    //cleanup(args, &q_mtx);
    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data somewhere...*/
    while(!done){
        lock(&queue->queue_lock);



        while(isEmpty(queue) && !done){ /* I try to check wether the queue is empty and there is still some file missing */
            cond_wait(&not_empty, &queue->queue_lock);
        }

        if(done){
            unlock(&queue->queue_lock);
            return NULL;
        }

        filename = dequeue(queue);
        printf("Thread %lu is working on file %s\n", pthread_self(), filename);
        unlock(&queue->queue_lock);


    }
    return NULL;
}
