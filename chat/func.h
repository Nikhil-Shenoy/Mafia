#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

typedef struct sockaddr SA;

void initializeSockets(int *playerfds, struct sockaddr_in *servaddr);
void handle_connection(int *playerfds,fd_set *read_set,SA *cliaddr, int *clilen,char *message);
void add_to_set(int *playerfds, fd_set *read_set);
