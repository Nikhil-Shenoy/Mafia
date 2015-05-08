#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "player.h"
#include "cliHelp.h"

// returns 1 if in list, 0 otherwise

int inList(char *name, Player **playerList) {

	int i;
	for(i = 0; i < 10; i++) {
		if((playerList[i] != NULL) && (strcmp(name,playerList[i]->name) == 0))
			return 1;
	}

	return 0;
}

void insert(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList) {
	
	int i;
	for(i = 0; i < 10; i++) {
		if(playerList[i] == NULL) {
			playerList[i] = (Player *)malloc(sizeof(Player));
			memset(playerList[i],'\0',sizeof(Player));
		
			// Fill in the fields for each player
			strcpy(playerList[i]->name,newPlayerMesg->name);
			strcpy(playerList[i]->message,newPlayerMesg->message);
			memcpy(&(playerList[i]->playerInfo),cliaddr,sizeof(*cliaddr));

			return;
		}
	}

}
