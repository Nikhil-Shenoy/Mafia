#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "player.h"
#include "client.h"

int inList(CliPacket *name, Player **playerList);
void insert(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList);

#endif
