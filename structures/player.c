#include <stdio.h>
#include <stdlib.h>
#include "../chat/util.h"
#include "player.h"

void init_player(Player *newPlayer) {
	newPlayer->alive = true;
	newPlayer->saved = false;
	newPlayer->fd = -1;
	memset(&(newPlayer->connInfo),'\0',sizeof(newPlayer->connInfo));
	newPlayer->name = NULL;
	newPlayer->role = ROLE_TOWNSPERSON;
	newPlayer->next = NULL;
}

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

Player *listFind(char *name, PlayerList *list) {
	Player *cur;
	cur = list->head;

	while(cur != NULL) {
		if(strcasecmp(name,cur->name) == 0)
			return cur;

		cur = cur->next;
	}

	return NULL;
}

void listPrint(PlayerList *list) {

	Player *cur = list->head;

	while(cur != NULL) {
		if(cur->alive)
			printf("\t%s\n",cur->name);

		cur = cur->next;
	}
	printf("\n");
}

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

void init_list(PlayerList *list) {
	list->head = NULL;
	list->size = 0;
}

void listApply(void (*a)(Player *p, void *aux),PlayerList *players, void *aux) {
	Player *cur = players->head;
	for(int i = 0; i < players->size; i++) {
		a(cur, aux);
		cur = cur->next;
	}
}

void listSend(PlayerList *players, char *msg, int length) {
	Player *cur = players->head;
	for(int i = 0; i < players->size; i++) {
		robustSend(cur->fd, msg, length);
		cur = cur->next;
	}
}
