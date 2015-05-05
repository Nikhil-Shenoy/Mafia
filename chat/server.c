#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAXLINE 2048
#define PORT 5000

int main(int argc, char *argv[]) {

	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM,0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	int bytesRead;
	socklen_t length; length = sizeof(cliaddr);
	char mesg[MAXLINE];

	printf("Starting the server...\n");
	while(1) {
		bytesRead = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)&cliaddr,&length);
		printf("Received %u bytes from client.\nMessage is: %s\n\n",bytesRead,mesg);
	
		sendto(sockfd,mesg,bytesRead,0,(struct sockaddr *)&cliaddr,length);
	}

	return 0;

}
