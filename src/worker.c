#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include "../lib/queue.h"
#include "../lib/enhanced_sc.h"


extern pthread_cond_t not_empty;
extern pthread_cond_t not_full;
extern struct sockaddr_un *saddr;


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

    while (fread(line, sizeof(line), 1, fp) != 0){
        result = i * *(line) + result;
        i++;
    }
    //printf("%ld\n", result);
    fclose(fp);
    return result;
}


int connect_wrapper(){
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    int ret_val;
    while(( ret_val = connect(fd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_un)) == -1) ){
        if (errno == ENOENT){
            printf("[CLIENT]:\tSocket not found, retrying...\n");
            sleep(2*0.01);
        }
        else exit(EXIT_FAILURE);
    }

    //printf("Created client socket with fd: %d\n", fd);
    return fd;
}


void *worker_function(void *args){

    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data to the server created by collector*/

    _queue *queue = (_queue *) args;
    char *filename;
    long int result;
    int fd;

    while(1){
        lock(&queue->q_lock);


        while(isEmpty(queue) && queue->done == 0){
            cond_wait(&not_empty, &queue->q_lock);
        }

        if(isEmpty(queue) && queue->done == 1){
            /*
             * No more things to do.
             * Thread send done message to server before exiting.
             */
            fd = connect_wrapper();
            writen(fd, "DONE", 5);
            close(fd);
            unlock(&queue->q_lock);
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

        //printf("%s\n", buffer);

        fd = connect_wrapper();
        //printf("Created client socket with fd: %d\n", fd);
        //printf("\033[1;34m[Thread]:\033[0m %ld \n\t [file]: %s \t [result]: %lld\n", pthread_self(), node.filename, node.result);
        writen(fd, buffer, strlen(buffer));
        close(fd);
        free(filename);
        free(buffer);
    }

}




