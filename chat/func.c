#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "func.h"

#define MAXLINE 2048
#define PORT 5000


void init_sockets(int *playerfds, struct sockaddr_in *servaddr) {

        int i;
        for(i = 0; i < 5; i++) {
                playerfds[i] = socket(AF_INET,SOCK_DGRAM,0);
                bind(playerfds[i],(SA *)servaddr,sizeof(*servaddr));
        }

}


void handle_connection(int *playerfds,fd_set *read_set,struct sockaddr_in *cliaddr, int *clilen,char *message) {

	int i, j, bytesRead;
	for(i = 0; i < 5; i++) {
		 if(FD_ISSET(playerfds[i],read_set)) {
                        bytesRead = recvfrom(playerfds[i],message,MAXLINE,0,(SA *)cliaddr,clilen);
                        printf("Received message on file descriptor %u\nMessage is: %s\n",playerfds[i],message);
			
			for(j = 0; j < 5; j++) 
	                        sendto(playerfds[j],message,MAXLINE,0,(SA *)cliaddr,*clilen);
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





