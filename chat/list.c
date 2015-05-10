#include "list.h"

// If newClient is in playerList, returns the index it's located at.
// Otherwise, returns -1.
int inList(CliPacket *newClient, Player **playerList, int len) {
	for(int i = 0; i < len; i++) {
		if((playerList[i] != NULL) &&
		   (strcmp(newClient->name,playerList[i]->name) == 0)) {
			return i;
		}
	}

	return -1;
}

void insert(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList, int len) {
	for(int i = 0; i < len; i++) {
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

void update(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList, int len) {
	int loc = inList(newPlayerMesg, playerList, len);
	if (loc == -1)
		insert(newPlayerMesg, cliaddr, playerList, len);
	else
		strncpy(playerList[loc]->message,newPlayerMesg->message, MAXLINE*2);
}
