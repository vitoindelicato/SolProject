#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/tools.h"

/* #include "../lib/queue.h"
 * including this will result in circular dependency and will give a lot of errors
 */

#define TD_POOL_SIZE  4
#define QUEUE_SIZE 8
#define TIME_DELAY 0


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
    _queue *queue = malloc(sizeof(_queue));
    queue->items = malloc(q_size * sizeof(char*));
    queue->size = q_size;
    printf("queue size: %d\n", queue->size);
    queue->front = 0;
    queue->rear = 0;

    for (int i = optind; i < argc; i++) {
        enqueue(queue, argv[i]);
    }

    if (dir_name != NULL){
        if( isDir(dir_name)){
            explorer(dir_name, queue);
        }
    }

    print_queue(queue);


    return 0;
}