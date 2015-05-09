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
	char recvline[2*MAXLINE];
	char name[MAXLINE];

	int chatDone; chatDone = 0; // 1 for yes, 0 for no
	int serverAccepts; serverAccepts = 1; // 1 for yes, 0 for no

	printf("Starting the client...\n");
	
	printf("What is your name? ");
	fgets(name,MAXLINE,stdin);

	// Get rid of the newline at the end of the name
	name[strlen(name)-1] = '\0';
	
	
	while(!chatDone) {

		int length; length = sizeof(servaddr);

		while(serverAccepts) {
			printf("--> ");
			fgets(mesg,MAXLINE,stdin);
			sendCliPacket(name,mesg,sockfd,&servaddr);

			bytesRead = recvfrom(sockfd,recvline,2*MAXLINE,0,(struct sockaddr *)&servaddr,&length);

			if(strcmp(recvline,"accepting") != 0)
				serverAccepts = 0;
			else			
				printf("Received %u bytes.\nResponse is %s\n\n",bytesRead,recvline);
		}
	
		while(strcmp(recvline,"end stream") != 0) {
			bytesRead = recvfrom(sockfd,recvline,2*MAXLINE,0,(struct sockaddr *)&servaddr,&length);
			if(strcmp(recvline,"end stream") != 0) {
				printf("%s\n",recvline);
				memset(recvline,'\0',2*MAXLINE);
			}
		}

		memset(recvline,'\0',2*MAXLINE);

		bytesRead = recvfrom(sockfd,recvline,2*MAXLINE,0,(struct sockaddr *)&servaddr,&length);
		if(strcmp(recvline,"accepting") == 0)
			serverAccepts = 1;

		

	}

	return 0;
}
