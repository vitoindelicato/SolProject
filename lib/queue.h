#include "enhanced_sc.h"
typedef struct{
    char **items;
    int front;
    int rear;
    int size;
    pthread_mutex_t queue_lock;
} _queue;


char *dequeue(_queue *queue);
void enqueue(_queue *queue, char *filename);
int isFull(_queue *queue);
int isEmpty(_queue *queue);
int get_size(_queue *queue);
void print_queue(_queue *queue);
