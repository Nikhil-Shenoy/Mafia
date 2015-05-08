#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "cliHelp.h"

#define MAXLINE 2048
#define PORT 5000

int main(int argc, char *argv[]) {

	int sockfd;
	struct sockaddr_in servaddr;

	char *host = "127.0.0.1";

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_aton(host,&servaddr.sin_addr);

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	
	int bytesRead;
	char mesg[MAXLINE];
	char recvline[MAXLINE];

	printf("Starting the client...\n");
	//while(fgets(mesg,MAXLINE,stdin) != NULL) {

	char name[MAXLINE];
	strcpy(name,"Shenoy");	
	while(1) {
		//sleep(3);
		printf("--> ");
		fgets(mesg,MAXLINE,stdin);
		//strcpy(mesg,"The other one\n");
		//sendto(sockfd,mesg,MAXLINE,0,(struct sockaddr *)&servaddr,sizeof(servaddr));
		sendCliPacket(name,mesg,sockfd,&servaddr);

		int length; length = sizeof(servaddr);
		bytesRead = recvfrom(sockfd,recvline,MAXLINE,0,(struct sockaddr *)&servaddr,&length);

		printf("Received %u bytes.\nResponse is %s\n\n",bytesRead,recvline);

	}

	return 0;
}
