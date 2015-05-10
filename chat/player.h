#ifndef PLAYER_H
#define PLAYER_H

#include <netinet/in.h>
#include "util.h"

typedef struct _player {
	char name[MAXLINE];
	char message[MAXLINE];
	struct sockaddr_in playerInfo;
} Player;

#endif
