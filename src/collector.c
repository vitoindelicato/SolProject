#include "../lib/enhanced_sc.h"
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include "../lib/node.h"

#define SOCKNAME "farm.sck"
#define MAX_CONNECTIONS 100


extern int n_threads;
socklen_t addrlen;


_node *node_builder(char *buffer){
    char *token;
    char *filename;
    long int result;

    token = strtok(buffer, ";");
    result = strtol(token, NULL, 10);
    token = strtok(NULL, ";");

    filename = token;

    _node *new_node = create_node(result, filename);
    return new_node;

}

_node *client_handler(int client_fd, int *stop, _node *head) {
    char buffer[PATH_MAX] = {'\0'};
    readn(client_fd, buffer, PATH_MAX);


    if(strcmp(buffer, "DONE") == 0) {
        //printf("Received DONE\n");
        close(client_fd);
        (*stop)++;
        return NULL;
    }

    if(strcmp(buffer, "PRINT") == 0) {
        //printf("Received PRINT\n");
        close(client_fd);
        printf("\nPrinting after SIGUSR1 =============:\n");
        print_list(head);
        printf("====================================\n\n");
        return NULL;
    }

    //printf("Received: %s\n", buffer);
    _node *new_node = node_builder(buffer);
    close(client_fd);
    return new_node;
}



int create_server_socket(struct sockaddr_un *saddr) {
    int fd, ret_val;

    /* Step1: create server socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }

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
    /* Collector is the server which handle requests from client.
     * client send data and the server operate to sort them in a linked list. */

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

    int stop = 0;
    _node *head = NULL;

    while(stop == 0){

        int new_client = accept(server_fd, (struct sockaddr *) &new_addr, &addrlen);

        if (new_client == -1) {
            if(errno == EINTR){
                continue;
            }else{
                fprintf(stderr, "accept failed [%s][%d]\n", strerror(errno), errno);
                break;
            }

        }
        else{
            _node *new_node = client_handler(new_client, &stop, head);

            if(new_node != NULL){
                insert_node(&head, new_node);
            }
        }
    }
    print_list(head);
    free_list(&head);
    unlink(SOCKNAME);
}