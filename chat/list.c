#include "list.h"

// If newClient is in playerList, returns the index it's located at.
// Otherwise, returns -1.
int inList(CliPacket *newClient, Player **playerList, int len) {
	for(int i = 0; i < len; i++) {
		if((playerList[i] != NULL) &&
		   streq(newClient->name,playerList[i]->name)) {
			return i;
		}
	}

	return -1;
}

// Add newPlayerMesg to playerList.
// Returns true on success, false on failure
bool insert(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList, int len) {
	for(int i = 0; i < len; i++) {
		if(playerList[i] == NULL) {
			playerList[i] = (Player *)malloc(sizeof(Player));
			memset(playerList[i],'\0',sizeof(Player));

			// Fill in the fields for each player
			strcpy(playerList[i]->name,newPlayerMesg->name);
			strcpy(playerList[i]->message,newPlayerMesg->message);
			memcpy(&(playerList[i]->playerInfo),cliaddr,sizeof(*cliaddr));

			return true;
		}
	}

	return false;
}

// Add newPlayerMesg to playerList if it's not already in there, or
// updates the player if it's already there.
// Returns true on success, false on failure
bool update(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList, int len) {
	int loc = inList(newPlayerMesg, playerList, len);
	if (loc == -1)
		return insert(newPlayerMesg, cliaddr, playerList, len);

	strncpy(playerList[loc]->message,newPlayerMesg->message, MAXLINE*2);
	return true;
}
