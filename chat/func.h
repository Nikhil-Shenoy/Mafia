#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "player.h"
#include "cliHelp.h"

typedef struct sockaddr SA;

void init_sockets(int *playerfds, struct sockaddr_in *servaddr);
void handle_connection(int *playerfds,fd_set *read_set,struct sockaddr_in *cliaddr, int *clilen,Player **playerList);
void add_to_set(int *playerfds, fd_set *read_set);
int max(int *playerfds);
void addClientToList(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList);
void initPlayerList(Player **playerList);

#endif
