#include "../lib/enhanced_sc.h"
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/epoll.h>
#include "../lib/node.h"
#include "../lib/socket_utils.h"

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
/*
_node *client_handler(int client_fd, int *stop, _node *head) {
    char buffer[PATH_MAX] = {'\0'};
    readn(client_fd, buffer, PATH_MAX);


    if(strcmp(buffer, "DONE") == 0) {
        //printf("Received DONE\n");
        //close(client_fd);
        (*stop)++;
        return NULL;
    }

    if(strcmp(buffer, "PRINT") == 0) {
        //printf("Received PRINT\n");
        //close(client_fd);
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
*/




void collector() {
    /* Collector is the server which handle requests from client.
     * client send data and the server operate to sort them in a linked list. */

    int server_fd;
    int events_count;
    int epoll_fd;

    char buffer[PATH_MAX] = {'\0'};
    int  n;
    _node *head = NULL;

    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKNAME);

    struct epoll_event event, events[n_threads]; // +2 rappresenting farm connection (for SIGUSR1) and master_worker connection (DONE message)



    /* Get the socket server fd */
    server_fd = create_server_socket(&server_addr); //socket --> bind --> listen
    if (server_fd == -1) {
        perror("Failed to create a server\n");
        exit(EXIT_FAILURE);
    }

    if(server_fd == -2){
        unlink(SOCKNAME);
        //printf("Retrying to create server socket...\n");
        server_fd = create_server_socket(&server_addr);
        if (server_fd == -1 || server_fd == -2) {
            perror("Failed to create a server\n");
            exit(EXIT_FAILURE);
        }
        /*
        else{
            printf("Server socket created successfully!\n");
        }*/
    }

    // Creazione epoll file_descriptor

    if ((epoll_fd = epoll_create(1)) == -1) {
        perror("Errore nella creazione del nuovo epoll file descriptor");
        exit(EXIT_FAILURE);
    }

    // aggiungo il socket del server al set dell'epoll
    event.data.fd = server_fd;
    event.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("Errore nell'aggiunta del server_fd all'epoll set");
        exit(EXIT_FAILURE);
    }

    int stop = 0;


    while(stop == 0){ /* Exiting while after master worker sends "DONE" message */


        events_count = epoll_wait(epoll_fd, events, MAX_CONNECTIONS, -1);
        for (int i = 0; i < events_count; i++){

            if(events[i].data.fd == server_fd) {
                /*Se ho un evento sul file descriptor del server
                 * allora significa che ho una nuova connessione in entrata*/

                int new_client = accept(server_fd, (struct sockaddr *) &server_addr, &addrlen);

                if (new_client == -1) {
                    if (errno == EINTR) {
                        continue;
                    } else {
                        fprintf(stderr, "accept failed [%s][%d]\n", strerror(errno), errno);
                        break;
                    }
                }

                /*Aggiungo il nuovo client al set di epoll*/
                event.data.fd = new_client;
                event.events = EPOLLIN;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &event) == -1) {
                    perror("Errore nell'aggiunta del client_fd all'epoll set");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                /*
                 * This branch means that an event occured on a file descriptor that isn't the server's one
                 * It means that a client sent something
                 */

                memset(buffer, 0, sizeof(buffer)); // cleaning buffer before reading
                n = read(events[i].data.fd, buffer, PATH_MAX);

                if(n == 0 ){
                    /* This means that client closed connection
                     * I can remove its file descriptor from set */
                    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1){
                        perror("Errore nella rimozione del client_fd dall'epoll set");
                        exit(EXIT_FAILURE);
                    }
                }
                else if(n > 0){
                    /* Client sent something */

                    if(strcmp(buffer, "PRINT") == 0) {
                        printf("\nPrinting after SIGUSR1 =============:\n");
                        print_list(head);
                        printf("====================================\n\n");
                        continue;
                    }
                    if(strcmp(buffer, "DONE") == 0) {
                        /* All threads returned, master worker sent last message, we're done, I can close the server */

                        if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1){
                            perror("Errore nella rimozione del client_fd dall'epoll set");
                            exit(EXIT_FAILURE);
                        }
                        stop++;
                        continue;
                    }
                    else{
                        _node *new_node = node_builder(buffer);
                        insert_node(&head, new_node);
                    }
                }
                else{
                    perror("Errore nella lettura del messaggio dal client");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    print_list(head);
    free_list(&head);
    close(server_fd);
    close(epoll_fd);
}