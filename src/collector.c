#include "../lib/enhanced_sc.h"
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include "../lib/node.h"

#define SOCKNAME "farm.sck"
#define MAX_CONNECTIONS 20


extern int n_threads;
/*
 * Collector has to:
 * Create socket
 * Handle incoming connections from threads
 * Read data
 * Sort data
 * Print data
 * Close everything should be closed
 */



void *client_handler(void *args) {
    char *buffer = calloc(PATH_MAX, sizeof(char));
    int fd = *(int *)args;
    //while(readn(fd, buffer, PATH_MAX)!=0){

    //}
    readn(fd, buffer, PATH_MAX);
    printf("Received: %s\n", buffer);
    free(buffer);
    return NULL;
}



int create_server_socket(struct sockaddr_un *saddr) {
    int fd, ret_val;

    /* Step1: create a TCP socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created server socket with fd: %d\n", fd);

    /* Initialize the socket address structure */



    /* Step2: bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)saddr, sizeof(struct sockaddr_un));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;

}

socklen_t addrlen;

void collector() {

    struct sockaddr_un new_addr;
    new_addr.sun_family = AF_UNIX;
    strcpy(new_addr.sun_path, SOCKNAME);

    int server_fd;
    /* Get the socket server fd */
    server_fd = create_server_socket(&new_addr); //socket --> bind --> listen
    if (server_fd == -1) {
        fprintf(stderr, "Failed to create a server\n");
        return;
    }



    while(1){ /* TODO: add conditions to this while*/
        int new_client = accept(server_fd, (struct sockaddr *) &new_addr, &addrlen);

        if (new_client == -1) {
            fprintf(stderr, "accept failed [%s]\n", strerror(errno));
            return;
        }
        else{
            pthread_t handler;
            create(&handler, NULL, client_handler, &new_client);
        }
        sleep(0.2); //can't figure out why this is needed

    }


    unlink(SOCKNAME);
}