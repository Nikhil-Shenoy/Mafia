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

static const char *roleActionStr[4] = {
	"suck",
	"kill",
	"investigate",
	"save"
};

struct player {
	bool alive;
	bool saved;
	struct sockaddr_in connInfo;
	int fd;
	char *name;
	enum role role;
	struct player *next;
	struct player *cur_vote; // a pointer to the accused player
	int kill_votes; // number of accusations against this player
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
int listSprint(char *buf, PlayerList *list);
void listDestroy(PlayerList *list);
void listApply(void (*a)(Player *p, void *aux),PlayerList *players, void *aux);
void listSend(PlayerList *players, char *msg, int length);
void listSendTo(PlayerList *players, Role r, char *msg, int length);
void listApplyTo(void (*a)(Player *p, void *aux),PlayerList *players, Role r, void *aux);
int listNumAlive(PlayerList *players);
int listNumAliveOf(PlayerList *players, Role r);

#endif
