#include "gameFlow.h"


int arrayMax(int *arr, int len) {
	int max = -1;
	for(int i = 0; i < len; i++) {
		if(arr[i] > max)
			max = arr[i];
	}

	return max;
}

void assignRoles(PlayerList *players) {

	int numPlayers = players->size;

	srand(time(NULL));

	int mafia = 0;
	int doctor = 0;
	int cop = 0;

	mafia = rand() % numPlayers;

	while(doctor == mafia)
		doctor = rand() % numPlayers;

	while((cop == doctor) || (cop == mafia))
		cop = rand() % numPlayers;

	Player *cur = players->head;

	int i = 0;
	while(i != mafia) {
		cur = cur->next;
		i++;
	}

	// Assign mafia's role
	cur->role = ROLE_MAFIA;

	// Assign cop's role
	i = 0;
	cur = players->head;
	while(i != cop) {
		cur = cur->next;
		i++;
	}

	cur->role = ROLE_COP;

	// Assign doctor's role
	i = 0;
	cur = players->head;
	while(i != doctor) {
		cur = cur->next;
		i++;
	}

	cur->role = ROLE_DOCTOR;
}

void describeRole(Player *p, void *aux) {
	(void)aux;

	char sendbuf[MAXLINE];
	int nbytes = sprintf(sendbuf, "%s: You are a %s\n",
						 p->name, roleStr[p->role]);

	robustSend(p->fd, sendbuf, nbytes);
}

void receiveAction(Player *p, void *aux) {
	PlayerList *players = aux;

	char sendbuf[MAXLINE];
	char recvbuf[MAXLINE];
	Player *toDoAThing = NULL;
	while(!toDoAThing) {
		int nbytes = sprintf(sendbuf, "Who do you want to %s?\n--> ",
							 roleActionStr[p->role]);
		robustSend(p->fd, sendbuf, nbytes);

		trimmedRecv(p->fd, recvbuf, sizeof recvbuf);
		toDoAThing = listFind(recvbuf, players);

		if(!toDoAThing) {
			debug("%s player %s chose to %s %s, who does not exist",
				  roleStr[p->role], p->name, roleActionStr[p->role], recvbuf);
			nbytes = sprintf(sendbuf, "Player %s does not exist\n", recvbuf);
			robustSend(p->fd, sendbuf, nbytes);
		} else {
			debug("%s player %s chose to %s %s.",
				  roleStr[p->role], p->name, roleActionStr[p->role], recvbuf);
		}
	}

	switch(p->role) {
	case ROLE_MAFIA: {
		char morning[] = "It is the dawn of a new day.\n";
		strcpy(sendbuf, morning);

		if(!(toDoAThing->saved)) {
			toDoAThing->alive = false;
			int nbytes = sprintf(sendbuf, "%sIt appears %s died amidst your slumber!\n", morning, recvbuf);
			listSend(players, sendbuf, nbytes);
		} else {
			char nobody_died[] = "Nobody died last night.\n";
			strcat(sendbuf, nobody_died);
			listSend(players, sendbuf, strlen(sendbuf));
		}
		break;
	}

	case ROLE_COP: {
		int nbytes = sprintf(sendbuf, "Player %s is %s mafioso.\n" , recvbuf,
							 (toDoAThing->role == ROLE_MAFIA)? "a" : "not a");
		robustSend(p->fd, sendbuf, nbytes);
		break;
	}

	case ROLE_DOCTOR:
		toDoAThing->saved = true;
		break;
	default:
		break;
	}
}

void doAction(PlayerList *players, Role r) {
	char living[players->size*MAXLINE];
	sprintf(living, "Hello %s! The living players are:\n", roleStr[r]);
	listSprint(living, players);
	listSendTo(players, r, living, strlen(living));

	listApplyTo(&receiveAction, players, r, (void *)players);
}

void collectVotes(PlayerList *players) {

	bool allVotesCollected = false;

	char *votes[players->size];
	int i = 0;
	/*
	while(!allVotesCollected) {
		receive a client's vote

		if(strncasecmp("cast vote",response,9) == 0) {
			extract vote
			votes[i] = vote;
		}

		if(i == players->size)
			allVotesCollected = true;
		else
			i++;
	}

	*/

	Player *cur;
	cur = players->head;
	i = 0;

	int counts[players->size];
	for(i = 0; i < players->size; i++)
		counts[i] = 0;

	// Collect frequency of votes

	i = 0;
	while(cur != NULL) {
		for(int j = 0; j < players->size; j++) {
			if(strncasecmp(votes[i],cur->name,strlen(cur->name)) == 0)
				counts[i]++;
		}

		cur = cur->next;
		i++;
	}

	int index = arrayMax(counts,players->size);

	cur = players->head;
	i = 0;
	while(i != index) {
		cur = cur->next;
		i++;
	}

	if(!(cur->saved))
		cur->alive = false;


}

void resetSaved(Player *p, void *aux) {
	(void)aux;

	p->saved = false;
}


void resetVote(Player *p, void *aux) {
	(void)aux;

	p->cur_vote = NULL;
	p->kill_votes = 0;
}

void printVotes(Player *p, void *aux) {
	PlayerList *players = aux;

	if (!p->alive)
		return;

	char sendbuf[MAXLINE];
	int nbytes = sprintf(sendbuf, "%s voted to kill %s.\n", p->name, p->cur_vote->name);

	listSend(players, sendbuf, nbytes);

	p->cur_vote->kill_votes++;
}
