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
#include "../structures/player.h"
#include "../structures/gameFlow.h"

static PlayerList *clients;
static int listenfd;

static bool keep_on_truckin = true;
char* welcome_message = "Welcome to Mafia!\nPlease enter your name: ";
char* night_message = "It is nighttime.\n";

int newConnection(fd_set *fdset, int listenfd) {
	int newfd = loggedAccept(listenfd);
	if (newfd == -1)
		return -1;

	robustSend(newfd, welcome_message, strlen(welcome_message));
	listInsert(newfd, clients);
	debug("Number of connected clients: %d", clients->size);

	FD_SET(newfd, fdset);
	return newfd;
}

struct broadcast_obj {
	char *msg;
	int sender;
	int nbytes;
};

void broadcastMessage(Player *p, void *aux) {
	struct broadcast_obj *args = aux;
	debug("Sending message to %d", p->fd);

	if (p->name &&
		p->fd != args->sender) {
		robustSend(p->fd, args->msg, args->nbytes);
	}
}

struct initLoop_obj {
	int listenfd;
	int *num_ready;
};

int initLoop(fd_set *fdset, int cur_fd, void *aux) {
	struct initLoop_obj args = *(struct initLoop_obj *)aux;

	// Either we have a new connection...
	if (cur_fd == args.listenfd)
		return newConnection(fdset, args.listenfd);

	// ...or we received a message
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

	Player *sender = clients->head;
	while(sender) {
		if (sender->fd == cur_fd) {
			break;
		}
		sender = sender->next;
	}
	check(sender, "Could not find client for fd %d", cur_fd);

	// If a player's name isn't set yet, do so now.
	if (sender->name == NULL) {
		recvbuf[nbytes-2] = '\0';
		sender->name = malloc(strlen(recvbuf));
		strcpy(sender->name, recvbuf);
		debug("FD %d is now %s.", cur_fd, sender->name);
	} else if (recvbuf[0] == '/') {
		if (strncmp(recvbuf+1, "ready", 5) == 0) {
			(* args.num_ready)++;
			debug("Number of readied clients: %d", (*args.num_ready));
		}
	} else {
		char sendbuf[MAXLINE*2];
		nbytes = sprintf(sendbuf, "%s: %s", sender->name, recvbuf);

		struct broadcast_obj broadcast_args = {
			.msg = sendbuf,
			.sender = cur_fd,
			.nbytes = nbytes
		};
		debug("Broadcasting message: %s", sendbuf);
		listApply(&broadcastMessage, clients, (void *) &broadcast_args);
	}
	return 0;
error:
	return -1;
}

int main(void)
{
	fd_set master;
	FD_ZERO(&master);

	clients = malloc(sizeof(*clients));
	init_list(clients);

	listenfd = open_listenfd(PORT);
	check((listenfd != -1), "failed to bind port");

	FD_SET(listenfd, &master);
	int fdmax = listenfd;

	// Start up a chat server that runs until everyone is ready
	int num_ready = 0;
	struct initLoop_obj initLoop_args = {
		.listenfd = listenfd,
		.num_ready = &num_ready
	};
	while(keep_on_truckin &&
		  (num_ready < READY_THRESHOLD ||
		   num_ready < clients->size)) {
		fdmax = fdloop(&master, fdmax, &initLoop, (void *)&initLoop_args);
	}

	bool game_over = false;
	assignRoles(clients);
	listApply(&describeRole, clients, NULL);

	// If we add more mafioso, we'd need to tell them who they are
	// here.

	while(keep_on_truckin && !game_over) {
		listSend(clients, night_message, strlen(night_message));
		whoWillYouKill(clients);
		return 0;
		// set all saved to false
	}

	return 0;
error:
	return 1;
}
