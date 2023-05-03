#ifndef COLLECTOR_H
    #define COLLECTOR_H
    #include <sys/socket.h>
    #include <sys/un.h>

    int create_server_socket(struct sockaddr_un *saddr);
    void collector();
#endif