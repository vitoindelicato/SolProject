#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "../lib/queue.h"
#include "../lib/enhanced_sc.h"
#include "../lib/node.h"


extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;


long calculator(_node *node){

    /* This function will be called by the thread function
    * It will open the file, and will make the calculations based on the data contained by the file
    */
    long int i = 0;
    long int result = 0;
    unsigned char line[8];

    FILE* fp;
    fp = fopen(node->filename, "rb");

    if (fp == NULL) {
        printf("Error while opening file %s", node->filename);
    }


    while (fread(line, sizeof(line), 1, fp) == 1){

        // Somma per ottenere il risoltato relativo ad ogni file
        //long number = *((long *)line);
        result = i * *(line) + result;
        i++;
    }
    //printf("%ld\n", result);
    fclose(fp);
    return result;
}



void *worker_function(void *args){

    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data somewhere...*/

    _queue *queue = (_queue *) args;
    _node node;
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

        if(filename == NULL && queue->done == 1) {
            unlock(&queue->q_lock);
            return (void *) 0;
        }

        unlock(&queue->q_lock);

        node.filename = NULL;
        node.filename = filename;


        node.result = calculator(&node);
        printf("\033[1;34m[Thread]:\033[0m %ld \n\t [file]: %s \t [result]: %lld\n", pthread_self(), node.filename, node.result);
        free(filename);
    }

}




