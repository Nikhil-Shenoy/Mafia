#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "func.h"
#include "player.h"
#include "util.h"

Player *playerList[PLAYERS];

int main(int argc, char *argv[]) {

	// TODO: Display IP address. Distribute to clients

	struct sockaddr_in servaddr, cliaddr;

	// Initialize server struct
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	fd_set read_set;
	int fdmax;

	// Create client sockets and bind
	int playerfds[PLAYERS];
	init_sockets(playerfds,&servaddr);

	// Initialize file descriptor set
	FD_ZERO(&read_set);

	fdmax = max(playerfds, PLAYERS) + 1;

	bzero(&cliaddr,sizeof(cliaddr));
	socklen_t clilen = sizeof(cliaddr);
	int nready; nready = 0;

	initPlayerList(playerList);

	for(; ;) {
		add_to_set(playerfds, PLAYERS, &read_set);

		if((nready = select(fdmax,&read_set,NULL,NULL,NULL)) < 0) {
			fprintf(stderr,"Error in select. Continuing...\n");
			continue;
		}

		handle_connection(playerfds, &read_set, (struct sockaddr_in *)&cliaddr, &clilen, playerList);

	}

	return 0;

}
