#include <sys/un.h>

int create_server_socket(struct sockaddr_un *saddr);
int client_connection(struct sockaddr_un *saddr);