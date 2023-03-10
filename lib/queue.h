#include "enhanced_sc.h"
typedef struct{
    char **items;
    int front;
    int rear;
    int size;
    int done;
    pthread_mutex_t q_lock;
} _queue;


char *dequeue(_queue *queue);
void enqueue(_queue *queue, char *filename);
int isFull(_queue *queue);
int isEmpty(_queue *queue);
