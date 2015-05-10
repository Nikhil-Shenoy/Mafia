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

void insert(char *name, PlayerList *list);
void remove(char *name, PlayerList *list);
Player *find(char *name);
void print(PlayerList *list);


