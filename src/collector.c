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
#include <signal.h>
#include <fcntl.h>
#include "../lib/node.h"

#define SOCKNAME "farm.sck"
#define MAX_CONNECTIONS 20


extern int n_threads;
socklen_t addrlen;
volatile __sig_atomic_t timeout = 0;
/*
 * Collector has to:
 * Create socket
 * Handle incoming connections from threads
 * Read data
 * Sort data
 * Print data
 * Close everything should be closed
 */



static void sigalrm_handler(int signum) {
    timeout = 1;
    printf("Received SIGALRM\n");
}

void *client_handler(void *args) {
    char *buffer = calloc(PATH_MAX, sizeof(char));
    int fd = *(int *)args;
    readn(fd, buffer, PATH_MAX);


    if(strcmp(buffer, "DONE") == 0) {
        printf("Received DONE\n");
        close(fd);
        timeout++;
        free(buffer);
        return NULL;
    }
    printf("Received: %s\n", buffer);
    free(buffer);
    close(fd);
    return NULL;
}



int create_server_socket(struct sockaddr_un *saddr) {
    int fd, ret_val;
    /* Step1: create server socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL,  O_NONBLOCK); /* I make the server non-blocking */
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created server socket with fd: %d\n", fd);


    /* Step2: bind the socket */
    ret_val = bind(fd, (struct sockaddr *)saddr, sizeof(struct sockaddr_un));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: listen for incoming connections */
    ret_val = listen(fd, MAX_CONNECTIONS);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;

}



void collector() {

    //int index = 0;
    //_node *array;
    //int done = 0;

    /* I will end loop of spwaning thread when timeout with alarm signalm will trigger
     * obviously timeout has to be greater than t_delay
     * */
    //int sig;

    /*
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));


    sa.sa_handler = sigalrm_handler;
    if ( sigaction(SIGALRM, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGALRM handler");
        exit(EXIT_FAILURE);
    }
     */


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



    pthread_t threadpool[100];
    int i = 0;

    while(timeout < n_threads -1){
        /* Because every thread will return 'DONE' after checking empty queue and done flag,
         * so I have to wait every worker that officially finish to work in order to avoid some data missing*/

        int new_client = accept(server_fd, (struct sockaddr *) &new_addr, &addrlen); /* Accept is non-blocking */

        if (new_client == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK){
                fprintf(stderr, "accept failed [%s][%d]\n", strerror(errno), errno);
                return;
            }
            else{
                continue;
            }
        }
        else{
            create(&threadpool[i], NULL, client_handler, &new_client);
            i++;
        }
        sleep(0.2); /*can't figure out why this is needed - without this readn returns null*/

    }

    for (int j = 0; j < i; j++) {
        join(threadpool[j], NULL);
    }


    unlink(SOCKNAME);
}