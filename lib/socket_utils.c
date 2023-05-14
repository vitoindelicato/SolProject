#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CONNECTIONS 100


int create_server_socket(struct sockaddr_un *server_addr){
    int fd, ret_val;

    /* Step1: create server socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }

    /* Step2: bind the socket */
    ret_val = bind(fd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_un));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -2;
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

int client_connection(struct sockaddr_un *client_addr){
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    int ret_val;
    while(( ret_val = connect(fd, (struct sockaddr*) client_addr, sizeof(struct sockaddr_un)) == -1) ){
        if (errno == ENOENT || errno == ECONNREFUSED) {

            printf("[CLIENT]:\tSocket not found, retrying...\n");
            sleep(2);
        }
        else {
            perror("Connection to server failed:");
            exit(EXIT_FAILURE);
        }
    }

    //printf("Created client socket with fd: %d\n", fd);

    return fd;
}
