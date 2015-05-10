#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "dbg.h"
#include "util.h"
#include "sock.h"
//#include "../structures/player.h"

// TEMPORARY
typedef struct player {
	bool alive;
	bool saved;
	struct sockaddr_in connInfo;
	int fd;
	char *name;
	char role[MAXLINE];
	struct player *next;
} Player;

Player *clients[PLAYERS];

static bool keep_on_truckin = true;
char* welcome_message = "Welcome to Mafia!\nPlease enter your name: ";

struct initLoop_obj {
	int listenfd;
	int *num_clients;
};

int initLoop(fd_set *fdset, int cur_fd, void *aux) {
	struct initLoop_obj args = *(struct initLoop_obj *)aux;
	debug("Number of connected clients: %d", *args.num_clients);

	// New connection
	if (cur_fd == args.listenfd) {
		int newfd = loggedAccept(args.listenfd);
		if (newfd == -1)
			return -1;

		robustSend(newfd, welcome_message, strlen(welcome_message));
		clients[*args.num_clients] = malloc(sizeof(Player));
		clients[*args.num_clients]->fd = newfd;
		clients[*args.num_clients]->name = NULL;
		(* args.num_clients)++;

		FD_SET(newfd, fdset);
		return newfd;
	}

	// Received message
	else {
		int nbytes;
		char recvbuf[MAXLINE];

		if ((nbytes = recv(cur_fd, recvbuf, sizeof recvbuf, 0)) <= 0) {
			// got error or connection closed by client
			if (nbytes == 0) {
				// connection closed
				log_info("socket %d hung up", cur_fd);
			} else {
				perror("recv");
			}
			close(cur_fd); // bye!
			FD_CLR(cur_fd, fdset); // remove from master set
			return -1;
		}

		Player *sender = NULL;
		for(int j = 0; j <= PLAYERS; j++ ) {
			if (clients[j]->fd == cur_fd) {
				sender = clients[j];
				break;
			}
		}
		check(sender, "Could not find client for fd %d", cur_fd);

		// If a player's name isn't set yet, do so now.
		if (sender->name == NULL) {
			recvbuf[nbytes-2] = '\0';
			sender->name = malloc(MAXLINE);
			strcpy(sender->name, recvbuf);
		} else {
			char sendbuf[MAXLINE*2];
			nbytes = sprintf(sendbuf, "%s: %s", sender->name, recvbuf);

			for(int j = 0; j <= PLAYERS; j++) {
				// Send message to everyone except the
				// server (listenfd) and the sender (i)
				if (clients[j] &&
					FD_ISSET(clients[j]->fd, fdset) &&
					clients[j]->fd != args.listenfd &&
					clients[j]->fd != cur_fd &&
					clients[j]->name) {
					robustSend(clients[j]->fd, sendbuf, nbytes);
				}
			}
		}
	}
	return 0;
error:
	return -1;
}

int main(void)
{
	fd_set master;
	FD_ZERO(&master);

	int listenfd = open_listenfd(PORT);
	check((listenfd != -1), "failed to bind port");

	FD_SET(listenfd, &master);
	int fdmax = listenfd;

	// Start up a chat server that runs until everyone is ready
	int num_clients = 0;
	struct initLoop_obj initLoop_args = {
		.listenfd = listenfd,
		.num_clients = &num_clients
	};
	while(keep_on_truckin && num_clients <= PLAYERS) {
		fdmax = fdloop(&master, fdmax, &initLoop, (void *)&initLoop_args);
	}

	return 0;
error:
	return 1;
}
