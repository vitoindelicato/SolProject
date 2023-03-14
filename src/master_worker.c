#include <stdlib.h>

#include "../lib/tools.h"
#include "worker.h"


extern int q_size;
extern int n_threads;
extern int t_delay;
extern char *dir_name;

extern pthread_cond_t not_empty;



void master_worker(int argc, char **argv, int optind){



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
        enqueue(queue, argv[i]);
    }


    if (dir_name != NULL) {
        if (isDir(dir_name)) {
            explorer(dir_name, queue);
            /* enqueue() is called recursively from explorer() */
        }
    }


    lock(&queue->q_lock);
    queue->done = 1;
    //printf("done setted and not_empty sent\n");

    /* At this point i will not insert any file into the queue
     * I need to send a not_empty signal in order to free all threads that where waiting
     * in (isEmpty && done == 0*/

    cond_broadcast(&not_empty);
    unlock(&queue->q_lock);

    for (int i = 0; i < n_threads; i++) {
        join(threadpool[i], NULL);
    }


    free(queue->items);
    free(queue);
    //printf("exiting main\n");
}
