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

int main(void)
{
	char recvbuf[MAXLINE];
	char sendbuf[MAXLINE*2];
	int nbytes;

	fd_set master;
	FD_ZERO(&master);
	fd_set read_fds;
	FD_ZERO(&read_fds);

	int listenfd = open_listenfd(PORT);
	check((listenfd != -1), "failed to bind port");

	FD_SET(listenfd, &master);
	int fdmax = listenfd;

	// Start up a chat server that runs until everyone is ready
	int num_clients = 0;
	while(keep_on_truckin && num_clients <= PLAYERS) {
		read_fds = master; // I don't really know why we need to do this
		check((select(fdmax+1, &read_fds, NULL, NULL, NULL) != -1), "select");

		// run through the existing connections looking for data to read
		for(int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				// New connection
				if (i == listenfd) {
					int newfd = loggedAccept(listenfd);
					if (newfd == -1)
						continue;

					FD_SET(newfd, &master); // add to master set
					if (newfd > fdmax) {	// keep track of the max
						fdmax = newfd;
					}

					robustSend(newfd, welcome_message, strlen(welcome_message));
					clients[num_clients] = malloc(sizeof(Player));
					clients[num_clients]->fd = newfd;
					clients[num_clients]->name = NULL;

					num_clients++;
				}

				// Received message
				else {
					if ((nbytes = recv(i, recvbuf, sizeof recvbuf, 0)) <= 0) {
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							log_info("socket %d hung up", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
						continue;
					}

					Player *sender = NULL;
					for(int j = 0; j <= PLAYERS; j++ ) {
						if (clients[j]->fd == i) {
							sender = clients[j];
							break;
						}
					}

					// If a player's name isn't set yet, do so now.
					if (sender->name == NULL) {
						recvbuf[nbytes-2] = '\0';
						sender->name = malloc(MAXLINE);
						strcpy(sender->name, recvbuf);
					} else {
						nbytes = sprintf(sendbuf, "%s: %s", sender->name, recvbuf);
						for(int j = 0; j <= PLAYERS; j++) {
							// Send message to everyone except the
							// server (listenfd) and the sender (i)
							if (clients[j] &&
								FD_ISSET(clients[j]->fd, &master) &&
								clients[j]->fd != listenfd &&
								clients[j]->fd != i &&
								clients[j]->name) {
								robustSend(clients[j]->fd, sendbuf, nbytes);
							}
						}
					}
				}
			}
		}
	}

	return 0;
error:
	return 1;
}
