#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../chat/util.h"
#include "../chat/sock.h"

typedef enum role {
	ROLE_TOWNSPERSON=0,
	ROLE_MAFIA,
	ROLE_COP,
	ROLE_DOCTOR
} Role;

static const char *roleStr[4] = {
	"Townsperson",
	"Mafioso",
	"Cop",
	"Doctor"
};

struct player {
	bool alive;
	bool saved;
	struct sockaddr_in connInfo;
	int fd;
	char *name;
	enum role role;
	struct player *next;
};

typedef struct player Player;

struct playerList {
	Player *head;
	int size;
};


typedef struct playerList PlayerList;

void init_player(Player *newPlayer);
void init_list(PlayerList *list);

void listInsert(int fd, PlayerList *list);
void listRemove(char *name, PlayerList *list);
Player *listFind(char *name, PlayerList *list);
void listPrint(PlayerList *list);
void listDestroy(PlayerList *list);
void listApply(void (*a)(Player *p, void *aux),PlayerList *players, void *aux);
void listSend(PlayerList *players, char *msg, int length);

#endif
