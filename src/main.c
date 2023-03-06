#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/tools.h"
#include "../lib/enhanced_sc.h"
#include "worker_function.h"

#define TD_POOL_SIZE  4
#define QUEUE_SIZE 8
#define TIME_DELAY 0


pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
// pthread_mutex_t q_mtx = PTHREAD_MUTEX_INITIALIZER;

int done = 0;

int main (int argc, char **argv){

    int opt;

    int n_threads = TD_POOL_SIZE;
    int q_size = QUEUE_SIZE;
    int t_delay = TIME_DELAY;

    char *dir_name = NULL;
    //char *filename;

    while((opt = getopt(argc, argv, "n:q:d:t:")) != -1) {

        switch (opt) {

            case 'n':
                n_threads = strtol(optarg, NULL, 10);
                printf("n_threads: %d\n", n_threads);
                break;

            case 'q':
                q_size = strtol(optarg, NULL, 10);
                printf("q_size: %d\n", q_size);
                break;

            case 't':
                t_delay = strtol(optarg, NULL, 10);
                printf("t_delay: %d\n", t_delay);
                break;

            case 'd':
                dir_name = optarg;
                printf("dir_name: %s\n", dir_name);
                break;

            case '?':
                printf("Unknown option: %c\nExiting...\n", optopt);
                exit(EXIT_FAILURE);

            default:
                printf("entered default \n");
                break;
        }

    }

    /* Queue init */
    _queue *queue = Malloc(sizeof(_queue));
    queue->items = Malloc(q_size * sizeof(char*));
    queue->size = q_size;
    mtx_init(&queue->queue_lock, NULL);
    printf("queue size: %d\n", queue->size);
    queue->front = 0;
    queue->rear = 0;


    /* Threadpool init */
    pthread_t threadpool[n_threads];


    for (int i = 0; i < n_threads; i++) {
        create(&threadpool[i], NULL, worker_function, (void *)queue);
    }


    for (int i = optind; i < argc; i++) {
        enqueue(queue, argv[i]); /* I was thinking to delegate producer tasks to queue library */
        /* I mean, if I can send signals from another file it would be goodly wrapped up in queue functions.*/
    }

    if (dir_name != NULL){
        if( isDir(dir_name)){
            explorer(dir_name, queue);
            /* enqueue() is called recursively from explorer()*/
            done = 1; /* This will stop the threadpool */
        }
    }
    done = 1;


    for (int i = 0; i < n_threads; i++) {
        join(threadpool[i], NULL);
    }





    return 0;
}