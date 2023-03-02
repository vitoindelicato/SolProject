#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/queue.h"


void dequeue(_queue *queue){

    if (isEmpty(queue)) {
        perror("Can't dequeue, queue is empty!");
        exit(EXIT_FAILURE);
    }

    printf("dequeueing: %s\n", queue->items[queue->rear]);
    queue->items[queue->rear] = NULL;
    queue->rear = (queue->rear + 1) % queue->size;

}


void enqueue(_queue *queue, char *filename){

    if (isFull(queue)) {
        /*If the queue is full, then I proceed to free the filename that was in the first position
         * I make the rear pointing the next cell
         * and I put the new filename in the cell that is now pointed by front index*/
        dequeue(queue);
        printf("enqueuing file: %s\n", filename);
        queue->items[queue->front] = filename;
        queue->front = (queue->front + 1) % queue->size;
    }
    else{
        printf("enqueuing file: %s\n", filename);
        queue->items[queue->front] = filename;
        queue->front = (queue->front + 1) % queue->size;

    }
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