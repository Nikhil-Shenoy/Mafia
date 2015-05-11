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
char* vote_message = "The town decides to sit down and have a good ol' fashioned lynching.\n"
	"Who will you vote for?\n";
char* tied_message = "There was a tie! You'll need to hold a new vote.\n";
char* mafia_victory_message = "The mafia wins!\n";
char* townspeople_victory_message = "The townspeople win!\n";
char* duplicate_name_message = "That name is already taken.\n";

// Author: Daniel
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

// Author: Daniel
void broadcastMessage(Player *p, void *aux) {
	struct broadcast_obj *args = aux;
	debug("Sending message to %d", p->fd);

	if (p->name &&
		p->fd != args->sender) {
		robustSend(p->fd, args->msg, args->nbytes);
	}
}

// Author: Daniel
bool readinessCommandler(Player *p, char *command, void *aux) {
	(void)p;
	int *num_ready = aux;
	if (strncmp(command, "ready", 5) == 0) {
		(* num_ready)++;
		debug("Number of readied clients: %d", *num_ready);
		return true;
	}
	return false;
}

// Author: Daniel
bool voteCommandler(Player *p, char *command, void *aux) {
	if (strlen(command) <= 5)
		return false;

	int *num_votes = aux;
	if (strncmp(command, "vote", 4) == 0) {
		Player *vote = listFind(command+5, clients);
		char sendbuf[MAXLINE];

		if (!vote) {
			debug("Player %s voted to kill %s, who does not exist", p->name, command+5);
			int nbytes = sprintf(sendbuf, "Player %s does not exist\n", command+5);
			robustSend(p->fd, sendbuf, nbytes);

		} else if (!vote->alive) {
			debug("Player %s voted to kill %s, who is already dead.", p->name, command+5);
			int nbytes = sprintf(sendbuf, "Player %s is already dead\n", command+5);
			robustSend(p->fd, sendbuf, nbytes);

		} else {
			if (p->cur_vote) {
				debug("Player %s changed their vote from %s to %s",
					  p->name, p->cur_vote->name, vote->name);
			} else {
				debug("Player %s voted to kill %s", p->name, vote->name);
				(* num_votes)++;
			}
			p->cur_vote = vote;
		}

		return true;
	}
	return false;
}

struct chatLoop_obj {
	void *aux;
	bool (*commandler)(Player *p, char *command, void *aux);
};

// Author: Daniel
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
		Player *original = listFind(recvbuf, clients);
		if (original) {
			robustSend(cur_fd, duplicate_name_message, strlen(duplicate_name_message));
		} else {
			sender->name = malloc(nbytes);
			strncpy(sender->name, recvbuf, nbytes);
			debug("FD %d is now %s.", cur_fd, sender->name);
		}
	} else if (recvbuf[0] == '/') {
		struct chatLoop_obj *args = aux;
		if (!args->commandler(sender, recvbuf+1, args->aux)) {
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
	bool tied;
};

// Author: Daniel
void findVoteTarget(Player *p, void *aux) {
	if(!p->alive)
		return;
	struct voteTarget_obj *args = aux;

	if(!args->max) {
		args->max = p;
	} else if (p->kill_votes > args->max->kill_votes) {
		args->max = p;
		args->tied = false;
	} else if (p->kill_votes == args->max->kill_votes) {
		args->tied = true;
	}
}

// Author: Daniel
bool winConditionCheck() {
	if (mafiaVictory(clients)) {
		listSend(clients, mafia_victory_message, strlen(mafia_victory_message));
		return true;
	}
	if (townspeopleVictory(clients)) {
		listSend(clients, townspeople_victory_message, strlen(townspeople_victory_message));
		return true;
	}
	return false;
}

// Author: Daniel
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

	char sendbuf[MAXLINE];
	while(keep_on_truckin && !game_over) {
		// Nighttime
		listSend(clients, night_message, strlen(night_message));
		doAction(clients, ROLE_DOCTOR);
		doAction(clients, ROLE_COP);
		doAction(clients, ROLE_MAFIA);
		listApply(&resetSaved, clients, NULL);

		if (winConditionCheck())
			return 0;

		// Daytime
		listSend(clients, vote_message, strlen(vote_message));
		memset(sendbuf,'\0',MAXLINE);
		listSprint(sendbuf, clients);
		listSend(clients, sendbuf, strlen(sendbuf));
		int num_votes;
		struct chatLoop_obj voteLoop_args = {
			.aux = (void *)&num_votes,
			.commandler = voteCommandler
		};
		struct voteTarget_obj voteTarget_args;
		do {
			//reset everything
			num_votes = 0;
			voteTarget_args.tied = false;
			voteTarget_args.max = NULL;
			debug("There are %d live players (out of %d).", listNumAlive(clients), clients->size);

			while(num_votes < listNumAlive(clients)) {
				fdmax = fdloop(&master, fdmax, listenfd,
							   &newConnection, &chatLoop, (void *)&voteLoop_args);
			}
			listApply(&printVotes, clients, (void *)clients);

			listApply(&findVoteTarget, clients, (void *)&voteTarget_args);
			if (voteTarget_args.tied) {
				listSend(clients, tied_message, strlen(tied_message));
			} else {
				voteTarget_args.max->alive = false;
				int nbytes = sprintf(sendbuf, "The town lynches %s\n", voteTarget_args.max->name);
				listSend(clients, sendbuf, nbytes);
			}

			listApply(&resetVote, clients, NULL);
		} while(voteTarget_args.tied);

		if (winConditionCheck())
			return 0;
	}

	return 0;
error:
	return 1;
}
