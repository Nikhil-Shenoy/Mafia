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

int newConnection(fd_set *fdset, int listenfd, void *aux) {
	(void)aux;
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

bool readinessCommandler(char *command, void *aux) {
	int *num_ready = aux;
	if (strncmp(command, "ready", 5) == 0) {
		(* num_ready)++;
		debug("Number of readied clients: %d", *num_ready);
		return true;
	}
	return false;
}

struct vote_obj {
	int *num_votes;
	Player *voter;
};

bool voteCommandler(char *command, void *aux) {
	if (strlen(command) <= 5)
		return false;

	struct vote_obj *args = aux;
	if (strncmp(command, "vote", 4) == 0) {
		Player *vote = listFind(command+5, clients);
		char sendbuf[MAXLINE];

		if (!vote) {
			debug("Player %s voted to kill %s, who does not exist", args->voter->name, command+5);
			int nbytes = sprintf(sendbuf, "Player %s does not exist\n", command+5);
			robustSend(args->voter->fd, sendbuf, nbytes);

		} else if (!vote->alive) {
			debug("Player %s voted to kill %s, who is already dead.", args->voter->name, command+5);
			int nbytes = sprintf(sendbuf, "Player %s is already dead\n", command+5);
			robustSend(args->voter->fd, sendbuf, nbytes);

		} else {
			if (args->voter->cur_vote) {
				debug("Player %s changed their vote from %s to %s",
					  args->voter->name, args->voter->cur_vote->name, vote->name);
			} else {
				debug("Player %s voted to kill %s", args->voter->name, vote->name);
				(* args->num_votes)++;
			}
			args->voter->cur_vote = vote;
		}

		return true;
	}
	return false;
}

struct chatLoop_obj {
	void *aux;
	bool (*commandler)(char *command, void *aux);
};

int chatLoop(fd_set *fdset, int cur_fd, void *aux) {
	int nbytes;
	char recvbuf[MAXLINE];

	Player *sender;
	for(sender = clients->head; sender; sender=sender->next) {
		if (sender->fd == cur_fd)
			break;
	}
	check(sender, "Could not find client for fd %d", cur_fd);

	if (!sender->alive) {
		return -1;
	}

	nbytes = trimmedRecv(cur_fd, recvbuf, sizeof(recvbuf));
	// got error or connection closed by client
	if (nbytes <= 0) {
		if (nbytes == 0) {
			// connection closed
			log_info("socket %d hung up", cur_fd);
		}
		close(cur_fd);
		FD_CLR(cur_fd, fdset); // remove from master set
		return -1;
	}

	// If a player's name isn't set yet, do so now.
	if (sender->name == NULL) {
		sender->name = malloc(nbytes);
		strncpy(sender->name, recvbuf, nbytes);
		debug("FD %d is now %s.", cur_fd, sender->name);
	} else if (recvbuf[0] == '/') {
		struct chatLoop_obj *args = aux;
		if (!args->commandler(recvbuf+1, args->aux)) {
			char invalid[] = "Invalid command.\n";;
			robustSend(cur_fd, invalid, strlen(invalid));
		}

	} else {
		char sendbuf[MAXLINE*2];
		nbytes = sprintf(sendbuf, "%s: %s\n", sender->name, recvbuf);

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

struct voteTarget_obj {
	Player *max;
	bool *tied;
};

void findVoteTarget(Player *p, void *aux) {
	struct voteTarget_obj *args = aux;

	if(!args->max) {
		args->max = p;
	} else if (p->kill_votes > args->max->kill_votes)
		args->max = p;
	else if (p->kill_votes == args->max->kill_votes)
		(*args->tied) = true;
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
	struct chatLoop_obj chatLoop_args = {
		.aux = (void *)&num_ready,
		.commandler = readinessCommandler
	};

	while(keep_on_truckin &&
		  (num_ready < READY_THRESHOLD ||
		   num_ready < clients->size)) {
		fdmax = fdloop(&master, fdmax, listenfd,
					   &newConnection, &chatLoop, (void *)&chatLoop_args);
	}

	bool game_over = false;
	assignRoles(clients);
	listApply(&describeRole, clients, NULL);

	// If we add more mafioso, we'd need to tell them who they are
	// here.

	while(keep_on_truckin && !game_over) {
		// Nighttime
		listSend(clients, night_message, strlen(night_message));
		doAction(clients, ROLE_DOCTOR);
		doAction(clients, ROLE_COP);
		doAction(clients, ROLE_MAFIA);
		listApply(&resetSaved, clients, NULL);

		// Daytime
		int num_votes;
		struct chatLoop_obj voteLoop_args = {
			.aux = (void *)&num_votes,
			.commandler = voteCommandler
		};
		bool tied = false;
		struct voteTarget_obj voteTarget_args = {
			.max = NULL,
			.tied = &tied
		};
		do {
			num_votes = 0;
			debug("There are %d live players (out of %d).", listNumAlive(clients), clients->size);
			while(num_votes < listNumAlive(clients)) {
				fdmax = fdloop(&master, fdmax, listenfd,
							   &newConnection, &chatLoop, (void *)&voteLoop_args);
			}
			listApply(&printVotes, clients, (void *)clients);

			voteTarget_args.max = NULL;
			listApply(&findVoteTarget, clients, (void *)&voteTarget_args);
			if (tied) {
				//tell peeps
			} else {
				voteTarget_args.max->alive = false;
				//tell peeps
			}

			listApply(&resetVote, clients, NULL);
		} while(tied);

		// Victory Condition check
		return 0;
	}

	return 0;
error:
	return 1;
}
