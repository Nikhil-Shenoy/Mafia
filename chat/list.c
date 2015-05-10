#include "list.h"

bool inList(CliPacket *newClient, Player **playerList) {
	for(int i = 0; i < PLAYERS; i++) {
		if((playerList[i] != NULL) &&
		   (strcmp(newClient->name,playerList[i]->name) == 0)) {
			return true;
		}
	}

	return false;
}

void insert(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList) {
	for(int i = 0; i < PLAYERS; i++) {
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


