#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "cliHelp.h"

#define MAXLINE 2048



void sendCliPacket(char *Name,char *Message, int sockfd, struct sockaddr_in *servaddr) {

	// Create the client packet
	CliPacket myMessage;
	memset(&myMessage,'\0',sizeof(myMessage));

	// Construct the packet
	strncpy(myMessage.name,Name,strlen(Name));
	strncpy(myMessage.message,Message,strlen(Message));

	sendto(sockfd,(const void *)&myMessage,sizeof(myMessage),0,(struct sockaddr *)servaddr,sizeof(*servaddr));

}
		

	
