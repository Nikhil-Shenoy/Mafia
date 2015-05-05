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
	struct sockaddr_in servaddr, cliaddr, temp;

	// Create a socket
	sockfd = socket(AF_INET, SOCK_DGRAM,0);

	// Initialize server struct	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Initialize temp struct	
	bzero(&temp, sizeof(temp));
	temp.sin_family = AF_INET;
	temp.sin_addr.s_addr = htonl(INADDR_ANY);
	temp.sin_port = htons(PORT);


	fd_set master, read_fd;
	int fdmax;

	// Initialize file descriptor sets
	FD_ZERO(&master);
	FD_ZERO(&read_set);
			
	// Bind the socket
	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0) {
		perror("Bind");
		close(sockfd);
		exit(1);
	}

	int playerfds[2];	
	playerfds[0] = socket(AF_INET,SOCK_DGRAM,0);
	bind(playerfds[0],(struct sockaddr *)&temp,sizeof(temp));
	FD_SET(playerfds[0],&master);

	playerfds[1] = socket(AF_INET,SOCK_DGRAM,0);
	bind(playerfds[1],(struct sockaddr *)&temp,sizeof(temp));
	FD_SET(playerfds[1],&master);

	fdmax = (playerfds[0] > playersfds[1]) ? playerfds[0] : playerfds[1];	

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
