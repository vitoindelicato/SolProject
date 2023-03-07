#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "enhanced_sc.h"

extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;


char *dequeue(_queue *queue){

    /* This function will be called only from threads
     * At the moment of the call the thread has already acquired the lock
     * So it is not necessary to do it again even here
     * Same thing even for unlock*/


    char *filename;
    filename = queue->items[queue->rear];
    printf("\033[1;31m[Dequeueing]:\033[0m %s\n", queue->items[queue->rear]);
    queue->items[queue->rear] = NULL;
    queue->rear = (queue->rear + 1) % queue->size;
    cond_signal(&not_full);
    return filename;


}


void enqueue(_queue *queue, char *filename){

    lock(&queue->queue_lock);

    while(isFull(queue)) {
        printf("isFull wait");
        cond_wait(&not_full, &queue->queue_lock);
    }

    /* dequeue() will be called only by threads */

    printf("\033[1;32m[Enqueueing]:\033[0m %s\n", filename);

    //printf("enqueuing file: %s\n", filename);
    queue->items[queue->front] = filename;
    queue->front = (queue->front + 1) % queue->size;

    if(queue->done == 1){
        printf("this should be last enqueue\n");
    }

    unlock(&queue->queue_lock);
    cond_signal(&not_empty);
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

    if( (queue->front) % queue->size == queue->rear && queue->items[queue->front] == NULL){
        return 1;
    }
    else{
        return 0;
    }
}
