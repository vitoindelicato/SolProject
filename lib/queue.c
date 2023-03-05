#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "queue.h"
#include "enhanced_sc.h"

extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;
extern pthread_mutex_t q_mtx;

char *dequeue(_queue *queue){

    /* This function will be called only from threads
     * At the moment of the call the thread has already acquired the lock
     * So it is not necessary to do it again even here
     * Same thing even for unlock*/

    char *filename = Malloc(sizeof(char) * 255);
    filename = queue->items[queue->rear];
    printf("dequeueing: %s\n", queue->items[queue->rear]);
    queue->items[queue->rear] = NULL;
    queue->rear = (queue->rear + 1) % queue->size;
    cond_signal(&not_full, &q_mtx);
    return filename;


}


void enqueue(_queue *queue, char *filename){

    lock(&q_mtx);

    while(isFull(queue)) {
        cond_wait(&not_full, &q_mtx);
    }

    /* dequeue() will be called only by threads */

    printf("enqueuing file: %s\n", filename);
    queue->items[queue->front] = filename;
    queue->front = (queue->front + 1) % queue->size;

    cond_signal(&not_empty, &q_mtx);
    unlock(&q_mtx);
}



int isFull(_queue *queue){
    /* I have a full circular tail if and only if
     * rear and front are pointing the same position
     * and that position is already hosting a file name
     * */

    if( (queue->front) % queue->size == queue->rear && queue->items[queue->rear] != NULL){
        return 1;
    }
    else{
        return 0;
    }
}

int isEmpty(_queue *queue){
    /* I have an empty circular tail if and only if
     * rear and front are pointing the same position
     * and that position is empty*/

    if( queue->front == queue->rear && queue->items[queue->front] == NULL){
        return 1;
    }
    return 0;
}


void print_queue(_queue *queue){
    printf("Printing queue:\n");
    for (int i = 0; i < queue->size; i++){
        if (queue->items[i] != NULL){
            printf("%s\n", queue->items[i]);
        }
        else{
            printf("NULL\n");
        }
    }
}