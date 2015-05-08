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


void init_sockets(int *playerfds, struct sockaddr_in *servaddr) {

        int i;
        for(i = 0; i < 5; i++) {
                playerfds[i] = socket(AF_INET,SOCK_DGRAM,0);
                bind(playerfds[i],(SA *)servaddr,sizeof(*servaddr));
        }

}


void handle_connection(int *playerfds,fd_set *read_set,struct sockaddr_in *cliaddr, int *clilen,Player **playerList) {


	int i, j, bytesRead;
	for(i = 0; i < 5; i++) {
		 if(FD_ISSET(playerfds[i],read_set)) {
			CliPacket cliMessage;
                        bytesRead = recvfrom(playerfds[i],&cliMessage,sizeof(cliMessage),0,(SA *)cliaddr,clilen);
                        printf("Received message on file descriptor %u\nMessage is: %s\n",playerfds[i],cliMessage.message);

			addClientToList(&cliMessage,cliaddr,playerList);	

			// For each descriptor, send a message to each player
			//int j;
			//for(j = 0; j < 10; j++)
		        	//sendto(playerfds[i],cliMessage.message,MAXLINE,0,(SA *)&(playerList[i]->playerInfo),sizeof(playerList[i]->playerInfo));
		}
	}

	for(i = 0; i < 10; i++) {
		if(playerList[i] != NULL) {
			for(j = 0; j < 5; j++) {
				if(FD_ISSET(playerfds[j],read_set)) {
					sendto(playerfds[j],playerList[i]->message,MAXLINE,0,(SA *)&(playerList[i]->playerInfo),sizeof(playerList[i]->playerInfo));
				}
			}
		}

	}

	return;
}

void add_to_set(int *playerfds, fd_set *read_set) {

	int i;
	for(i = 0; i < 5; i++) 
		FD_SET(playerfds[i],read_set);

	return;
}

int max(int *playerfds) {
	int max; max = -1;
	int i;
	for(i = 0; i < 5; i++) {
		if(playerfds[i] > max)
			max = playerfds[i];
	}

	return max;	
}

void initPlayerList(struct sockaddr_in **playerList) {

	int i; 
	for(i = 0; i < PLAYERS; i++) 
		playerList[i] = NULL;
	
}

void addClientToList(CliPacket *newPlayerMesg, struct sockaddr_in *cliaddr, Player **playerList) {

	/*
			If player is not in the list, add him to the first Null spot
			Else, return
	*/


	int i;
	for(i = 0; i < PLAYERS; i++) {
		
		if(inList(newPlayerMesg->name,playerList))
			return;
		else {
			insert(newPlayerMesg,cliaddr,playerList);
			return;
		}

		
	}

}
