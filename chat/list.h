#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "player.h"
#include "client.h"

int inList(CliPacket *name, Player **playerList, int len);
void insert(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList, int len);
void update(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList, int len);

#endif
