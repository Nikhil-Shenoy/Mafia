#include <stdio.h>
#include "../chat/util.h"
#include "player.h"

void init_player(Player *newPlayer) {
	newPlayer->alive = true;
	newPlayer->saved = false;
	fd = -1;
	memset(&(newPlayer->connInfo),'\0',sizeof(newPlayer->connInfo));
	memset(newPlayer->name,'\0',MAXLINE);
	memset(newPlayer->role,'\0',MAXLINE);
	newPlayer->next = NULL;
}

void insert(char *name, PlayerList *list) {

	Player *newPlayer = (Player *)malloc(sizeof(Player));
	init_player(newPlayer);

	strcpy(newPlayer->name,name);

	Player *cur = list->head;

	if(cur == NULL) 
		list->head = newPlayer;
	else
		while(cur->next != NULL)
			cur = cur->next;

		cur->next = newPlayer;
}

void remove(char *name, PlayerLlist *list) {

	Player *cur = list->head;

	if(cur == NULL) {
		printf("No players in the list\n");
		return;
	}

	while((strcmp(name,cur->name) != 0) && (cur != NULL))
		cur = cur->next;

	
		

	
