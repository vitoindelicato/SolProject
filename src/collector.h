#include <sys/socket.h>
#include <sys/un.h>

int create_server_socket(struct sockaddr_un *saddr);
void collector();