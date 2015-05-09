#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "func.h"
#include "cliHelp.h"
#include "player.h"
#include "list.h"

#define MAXLINE 2048
#define PORT 5000
#define PLAYERS 10
#define MSGCOUNT 5

char group[MSGCOUNT][2*MAXLINE];
int groupCount;

void init_sockets(int *playerfds, struct sockaddr_in *servaddr) {

	int i;
	for(i = 0; i < PLAYERS; i++) {
		playerfds[i] = socket(AF_INET,SOCK_DGRAM,0);
		bind(playerfds[i],(SA *)servaddr,sizeof(*servaddr));
	}

	memset(group,'\0',10*MAXLINE);
	groupCount = 0;
}

void handle_connection(int *playerfds, fd_set *read_set,struct sockaddr_in *cliaddr, int *clilen,Player **playerList) {

	int i, j, bytesRead;

	while(groupCount < MSGCOUNT) {

		for(i = 0; i < PLAYERS; i++) {
			if(FD_ISSET(playerfds[i],read_set)) {
				CliPacket cliMessage;
				bytesRead = recvfrom(playerfds[i], &cliMessage, sizeof(cliMessage), 0,(SA *)cliaddr, clilen);
				printf("Received message on file descriptor %i\n"
				       "Message is: %s\n",
				       playerfds[i],cliMessage.message);
				sprintf(group[groupCount],"\t%s: %s",cliMessage.name,cliMessage.message);

				int clilen; clilen = sizeof(*cliaddr);
				char *acceptMsg = (groupCount == MSGCOUNT - 1)? "not accepting" : "accepting";
				sendto(playerfds[i], acceptMsg, strlen(acceptMsg), 0, (SA *)cliaddr, clilen);
				groupCount++;

				addClientToList(&cliMessage,cliaddr,playerList);

			}
		}
	}

	for(i = 0; i < 10; i++) {
		for(j = 0; j < 5; j++) {
			if((playerList[i] != NULL) &&  (FD_ISSET(playerfds[j],read_set))){
				int k;
				for(k = 0; k < 5; k++) {
					sendto(playerfds[j],group[k],2*MAXLINE,0,(SA *)&(playerList[i]->playerInfo),sizeof(playerList[i]->playerInfo));
				}
				sendto(playerfds[j],"end stream",strlen("end stream"),0,(SA *)&(playerList[i]->playerInfo),sizeof(playerList[i]->playerInfo));
				sendto(playerfds[j],"accepting",strlen("accepting"),0,(SA *)&(playerList[i]->playerInfo),sizeof(playerList[i]->playerInfo));

			}
		}
	}

	groupCount = 0;
	memset(group,'\0',5*2*MAXLINE);

	return;
}

void add_to_set(int *playerfds, int len, fd_set *read_set) {
	int i;
	for(i = 0; i < len; i++)
		FD_SET(playerfds[i],read_set);

	return;
}

int max(int *arr, int len) {
	int max = -1;
	int i;
	for(i = 0; i < len; i++) {
		if(arr[i] > max)
			max = arr[i];
	}

	return max;
}

void initPlayerList(struct sockaddr_in **playerList) {
	int i;
	for(i = 0; i < PLAYERS; i++)
		playerList[i] = NULL;
}

// If player is not in the list, add him to the first Null spot.
void addClientToList(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList) {

	if(!inList(newPlayerMesg,playerList))
		insert(newPlayerMesg,cliaddr,playerList);
}
