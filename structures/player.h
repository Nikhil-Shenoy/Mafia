#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../chat/util.h"

typedef enum role {
	ROLE_TOWNSPERSON=0,
	ROLE_MAFIA,
	ROLE_COP,
	ROLE_DOCTOR
} Role;


struct player {
	bool alive;
	bool saved;
	struct sockaddr_in connInfo;
	int fd;
	char name[MAXLINE];
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

void listInsert(char *name, PlayerList *list);
void listRemove(char *name, PlayerList *list);
Player *listFind(char *name, PlayerList *list);
void listPrint(PlayerList *list);
void listDestroy(PlayerList *list);


