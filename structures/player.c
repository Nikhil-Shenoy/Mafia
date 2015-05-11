#include <stdio.h>
#include <stdlib.h>
#include "../chat/util.h"
#include "player.h"

// Author: Nikhil
void init_player(Player *newPlayer) {
	newPlayer->alive = true;
	newPlayer->saved = false;
	newPlayer->fd = -1;
	memset(&(newPlayer->connInfo),'\0',sizeof(newPlayer->connInfo));
	newPlayer->name = NULL;
	newPlayer->role = ROLE_TOWNSPERSON;
	newPlayer->next = NULL;
	newPlayer->cur_vote = NULL;
	newPlayer->kill_votes = 0;
}

// Author: Nikhil
void listInsert(int fd, PlayerList *list) {

	Player *newPlayer = (Player *)malloc(sizeof(Player));
	init_player(newPlayer);

	newPlayer->fd = fd;

	Player *cur = list->head;

	if(cur == NULL) {
		list->head = newPlayer;
		list->size++;
	} else {
		while(cur->next != NULL)
			cur = cur->next;

		cur->next = newPlayer;
		list->size++;
	}
}

// Author: Nikhil
void listRemove(char *name, PlayerList *list) {

	Player *cur = list->head;
	Player *prev = list->head;
	cur = cur->next;

	while((strcmp(name,cur->name) != 0) && (cur != NULL)) {
		cur = cur->next;
		prev = prev->next;
	}

	if(cur == NULL) {
		printf("No players in the list\n");
		return;
	} else if(cur->next == NULL) {
		prev->next = NULL;
		cur->next = NULL;
		free(cur);
		return;
	} else {
		prev->next = cur->next;
		cur->next = NULL;
		free(cur);
		return;
	}
}

// Author: Nikhil
Player *listFind(char *name, PlayerList *list) {
	for(Player *cur = list->head; cur != NULL; cur = cur->next) {
		if(strcasecmp(name,cur->name) == 0)
			return cur;
	}

	return NULL;
}

// Author: Nikhil
void listPrint(PlayerList *list) {
	for(Player *cur = list->head; cur != NULL; cur = cur->next) {
		if(cur->alive)
			printf("\t%s\n",cur->name);
	}
}

// Author: Daniel
int listSprint(char *buf, PlayerList *list) {
	int nbytes = strlen(buf);
	for(Player *cur = list->head; cur != NULL; cur = cur->next) {
		if(cur->alive) {
			strcat(buf, "\t");
			strcat(buf, cur->name);
			strcat(buf, "\n");
			nbytes += strlen(cur->name) + 2;
		}
	}
	return nbytes;
}


// Author: Nikhil
void listDestroy(PlayerList *list) {

	Player *cur = list->head;
	cur = cur->next;
	Player *prev = list->head;

	while(cur != NULL) {
		list->head = cur;
		prev->next = NULL;
		free(prev);
		prev = cur;
		cur = cur->next;
	}
}

// Author: Nikhil
void init_list(PlayerList *list) {
	list->head = NULL;
	list->size = 0;
}

// Author: Nikhil
void listApply(void (*a)(Player *p, void *aux),PlayerList *players, void *aux) {
	for(Player *cur = players->head; cur; cur = cur->next) {
		a(cur, aux);
	}
}

// Author: Daniel
void listSend(PlayerList *players, char *msg, int length) {
	for(Player *cur = players->head; cur; cur = cur->next) {
		robustSend(cur->fd, msg, length);
	}
}

// Author: Daniel
void listApplyTo(void (*a)(Player *p, void *aux), PlayerList *players, Role r, void *aux) {
	for(Player *cur = players->head; cur; cur = cur->next) {
		if (cur->role != r)
			continue;
		a(cur, aux);
	}
}

// Author: Daniel
void listSendTo(PlayerList *players, Role r, char *msg, int length) {
	for(Player *cur = players->head; cur; cur = cur->next) {
		if (cur->role != r)
			continue;
		robustSend(cur->fd, msg, length);
	}
}

// Author: Daniel
void __alive(Player *p, void *aux) {
	int *num_alive = aux;
	if (p->alive)
		(*num_alive)++;
}

// Author: Daniel
int listNumAlive(PlayerList *players) {
	int num_alive = 0;
	listApply(&__alive, players, (void *)&num_alive);
	return num_alive;
}

// Author: Daniel
int listNumAliveOf(PlayerList *players, Role r) {
	int num_alive = 0;
	listApplyTo(&__alive, players, r, (void *)&num_alive);
	return num_alive;
}
