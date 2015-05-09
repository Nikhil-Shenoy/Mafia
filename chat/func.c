#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include "func.h"
#include "cliHelp.h"
#include "player.h"
#include "list.h"

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

void handle_connection(int *playerfds, fd_set *read_set,struct sockaddr_in *cliaddr, socklen_t *clilen,Player **playerList) {
	int bytesRead;

	while(groupCount < MSGCOUNT) {
		for(int i = 0; i < PLAYERS; i++) {
			if(FD_ISSET(playerfds[i],read_set)) {
				CliPacket cliMessage;

				bytesRead = recvfrom(playerfds[i], &cliMessage, sizeof(cliMessage), 0,(SA *)cliaddr, clilen);
				printf("Received message on file descriptor %i\n"
				       "Message is: %s\n",
				       playerfds[i],cliMessage.message);
				if(!isalnum(cliMessage.message[0]))
					continue;

				sprintf(group[groupCount],"\t%s: %s",cliMessage.name,cliMessage.message);

				int clilen; clilen = sizeof(*cliaddr);
				char *acceptMsg = (groupCount == MSGCOUNT - 1)? "not accepting" : "accepting";
				sendto(playerfds[i], acceptMsg, strlen(acceptMsg), 0, (SA *)cliaddr, clilen);
				groupCount++;

				addClientToList(&cliMessage,cliaddr,playerList);

			}
		}
	}

	// Broadcast the stored messages

	struct sockaddr_in sock_in = {
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(PORT+2),
		.sin_family = AF_INET
	};
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	int status;
	//status = bind(sock, (struct sockaddr *)&sock_in, sizeof(struct sockaddr_in));
	//printf("Bind Status = %d\n", status);

	int on = 1;
	status = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int) );
	printf("Setsockopt Status = %d\n", status);

	sock_in.sin_addr.s_addr=htonl(-1); /* send message to 255.255.255.255 */
	sock_in.sin_port = htons(PORT+1); /* port number */
	sock_in.sin_family = AF_INET;

	for(int i = 0; i < PLAYERS; i++) {
		status = sendto(sock, group[i],2*MAXLINE + 10 , 0, (struct sockaddr *)&sock_in, sizeof(sock_in));
		printf("sendto Status = %d\n", status);

		// Verify that client has received the broadcast? Maybe use receive and check every packet	
	}

	status = sendto(sock,"end stream", strlen("end stream"), 0, (struct sockaddr *)&sock_in, sizeof(sock_in));
	printf("sendto Status = %d\n", status);

	status = sendto(sock,"accepting", strlen("accepting"), 0, (struct sockaddr *)&sock_in, sizeof(sock_in));
	printf("sendto Status = %d\n", status);


	/*
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 5; j++) {
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
	*/

	groupCount = 0;
	memset(group,'\0',PLAYERS*2*MAXLINE);
	//shutdown(sock,2);
	close(sock);
	return;
}

void add_to_set(int *playerfds, int len, fd_set *read_set) {
	for(int i = 0; i < len; i++)
		FD_SET(playerfds[i],read_set);

	return;
}

int max(int *arr, int len) {
	int max = -1;
	for(int i = 0; i < len; i++) {
		if(arr[i] > max)
			max = arr[i];
	}

	return max;
}

void initPlayerList(Player **playerList) {
	for(int i = 0; i < PLAYERS; i++)
		playerList[i] = NULL;
}

// If player is not in the list, add him to the first Null spot.
void addClientToList(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList) {

	if(!inList(newPlayerMesg,playerList))
		insert(newPlayerMesg,cliaddr,playerList);
}
