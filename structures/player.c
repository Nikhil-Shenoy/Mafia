#include <stdio.h>
#include <stdlib.h>
#include "../chat/util.h"
#include "player.h"

void init_player(Player *newPlayer) {
	newPlayer->alive = true;
	newPlayer->saved = false;
	newPlayer->fd = -1;
	memset(&(newPlayer->connInfo),'\0',sizeof(newPlayer->connInfo));
	memset(newPlayer->name,'\0',MAXLINE);
	memset(newPlayer->role,'\0',MAXLINE);
	newPlayer->next = NULL;
}

void listInsert(char *name, PlayerList *list) {

	Player *newPlayer = (Player *)malloc(sizeof(Player));
	init_player(newPlayer);

	strcpy(newPlayer->name,name);

	Player *cur = list->head;

	if(cur == NULL) 
		list->head = newPlayer;
	else {
		while(cur->next != NULL)
			cur = cur->next;

		cur->next = newPlayer;
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








	
