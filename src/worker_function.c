#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "../lib/queue.h"
#include "../lib/enhanced_sc.h"
#include "../lib/node.h"


extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;


void calculator(_node *node){

    /* This function will be called by the thread function
    * It will open the file, and will make the calculations based on the data contained by the file
    */
    long i = 0;
    //long *result = (long *) malloc(sizeof(long));
    char line[100] = {'\0'};

    FILE* fp;
    fp = fopen(node->filename, "rb");

    if (fp == NULL) {
        printf("Error while opening file %s", node->filename);
    }

    while( fgets(line, 100, fp) != NULL){
        //printf("%s", line);

        *node->result = *node->result + ( i *  strtol(line, NULL, 10));
        i++;
    }
    fclose(fp);
}



void *worker_function(void *args){

    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data somewhere...*/

    _queue *queue = (_queue *) args;
    _node *node = Malloc(sizeof(_node));
    char *filename;

    while(1){
        lock(&queue->q_lock);


        while(isEmpty(queue) && queue->done == 0){
            cond_wait(&not_empty, &queue->q_lock);
        }

        if(isEmpty(queue) && queue->done == 1){
            unlock(&queue->q_lock);;

            return NULL;
        }

        filename = dequeue(queue);

        if(filename == NULL && queue->done == 1){
            unlock(&queue->q_lock);
            return (void*)0;
        }

        unlock(&queue->q_lock);


        node->filename = filename;


        calculator(node);
        printf("\033[1;34m[Thread]:\033[0m %ld \n\t [file]: %s \t [result]: %ld\n", pthread_self(), node->filename, *(node->result));
        free(filename);
        //free(node->filename);
        //free(node);

    }

}




