#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>
#include "../chat/util.h"

struct player {
	bool alive;
	bool saved;
	struct sockaddr_in connInfo;
	int fd;
	char name[MAXLINE];
	char role[MAXLINE];
	struct player *next;
};

struct playerList PlayerList {
	Player *head;
	int size;
};


typedef struct player Player;
typedef struct playerList PlayerList;
	
void init_player(Player *newPlayer);

void listInsert(char *name, PlayerList *list);
void listRemove(char *name, PlayerList *list);
Player *listFind(char *name);
void listPrint(PlayerList *list);


