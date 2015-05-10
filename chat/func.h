#ifndef FUNC_H
#define FUNC_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client.h"
#include "dbg.h"
#include "func.h"
#include "list.h"
#include "player.h"
#include "util.h"

typedef struct sockaddr SA;

void init_sockets(int *playerfds, struct sockaddr_in *servaddr);
void handle_connection(int *playerfds, fd_set *read_set, struct sockaddr_in *cliaddr, socklen_t *clilen, Player **playerList);
void add_to_set(int *playerfds, int len, fd_set *read_set);
int max(int *arr, int len);
void initPlayerList(Player **playerList);

#endif
