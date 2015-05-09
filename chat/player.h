#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "vars.h"

struct player {
	char name[MAXLINE];	
	char message[MAXLINE];
	struct sockaddr_in playerInfo;
};

typedef struct player Player;

#endif
