#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../lib/queue.h"
#include "../lib/enhanced_sc.h"


extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;

void *worker_function(void *args){

    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data somewhere...*/

    _queue *queue = (_queue *) args;
    char *filename;
    // pthread_cleanup_push(cleanup_handler, args, &queue->q_lock);

    while(1){
        lock(&queue->q_lock);


        while(isEmpty(queue) && queue->done == 0){
            printf("isEmpty wait \n");
            cond_wait(&not_empty, &queue->q_lock);
        }

        if(isEmpty(queue) && queue->done == 1){
            unlock(&queue->q_lock);
            return NULL;
        }

        filename = dequeue(queue);

        if(filename == NULL && queue->done == 1){
            unlock(&queue->q_lock);
            return (void*)0;
        }

        printf("\033[1;34m[Thread]:\033[0m %ld is working on file %s\n", pthread_self(), filename);
        unlock(&queue->q_lock);


    }

}

