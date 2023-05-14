#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>
#include "../lib/queue.h"
#include "../lib/socket_utils.h"


extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;
extern int t_delay;

extern struct sockaddr_un client_addr;

long calculator(char *filename){

    /* This function will be called by the thread function
    * It will open the file, and will make the calculations based on the data contained by the file
    */

    long int i = 0;
    long int result = 0;
    unsigned char line[8];

    FILE* fp;
    fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("Error while opening file %s", filename);
    }

    while (fread(line, sizeof(line), 1, fp) == 1){
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
     * then it will send the data to the server created by collector*/

    _queue *queue = (_queue *) args;
    char *filename;
    long int result;
    int fd = client_connection(&client_addr);

    while(1){
        lock(&queue->q_lock);


        while(isEmpty(queue)){
            if(queue->done == 1){
                unlock(&queue->q_lock);
                close(fd);
                return (void*) 0;
            }
            cond_wait(&not_empty, &queue->q_lock);
        }

        if(isEmpty(queue) && queue->done == 1){
            /*
             * No more things to do.
             * Unlocking resources and exiting.
             */

            unlock(&queue->q_lock);
            close(fd);
            return (void*) 0;
        }

        filename = dequeue(queue);
        unlock(&queue->q_lock);

        result = calculator(filename);
        int digits = floor(log10(result) + 1);

        char *buffer = Malloc(sizeof(char) * (digits + strlen(filename) + 2 ));
        snprintf(buffer, digits+1, "%ld", result);
        strncat(buffer, ";", 2);
        strncat(buffer, filename, strlen(filename));
        strncat(buffer, "\0", 2);

        int sent_bytes = write(fd, buffer, strlen(buffer));
        if (sent_bytes == -1){
            perror("Error on write");
            return (void*) 0;
        }

        free(filename);
        free(buffer);
        sleep(t_delay * 0.001);
    }
}




