#include "gameFlow.h"


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

void whoWillYouKill(PlayerList *players) {

	char mafiaMesg[] = "Hello Mafioso! The living players are:\n";
	
	char living[players->size][MAXLINE];
	memset(living,'\0',(players->size)*MAXLINE);


	Player *cur = players->head;
	int i;
	for(i = 0; i < players->size; cur = cur->next) {
		living[i][0] = '\t';
		strcat(living[i],cur->name);
		i++;
	}

	char command[] = "\nWho do you want to kill?\n--> ";

	// Insert receive command for processing

	Player *toKill = listFind(/*name*/,players);
	if(!(toKill->saved))
		toKill->alive = false;

}

void whoWillYouSave(PlayerList *players) {

	char doctorMesg[] = "Hello Doctor! The living players are:\n";
	
	char living[players->size][MAXLINE];
	memset(living,'\0',(players->size)*MAXLINE);


	Player *cur = players->head;
	int i;
	for(i = 0; i < players->size; cur = cur->next) {
		living[i][0] = '\t';
		strcat(living[i],cur->name);
		i++;
	}

	char command[] = "\nWho do you want to save?\n--> ";

	// Insert receive command for processing
	
	Player *toSave = listFind(/*name*/,players);
	toSave->saved = true;

	return;
}

void whoWillYouInvestigate(PlayerList *players) {
	
	char copMesg[] = "Hello Cop! The living players are:\n";
	
	char living[players->size][MAXLINE];
	memset(living,'\0',(players->size)*MAXLINE);


	Player *cur = players->head;
	int i;
	for(i = 0; i < players->size; cur = cur->next) {
		living[i][0] = '\t';
		strcat(living[i],cur->name);
		i++;
	}

	char command[] = "\nWho do you want to investigate?\n--> ";

	// Insert receive command for processing
	
	Player *toInvestigate = listFind(/*name*/,players);

	if(toInvestigate->role == ROLE_MAFIA)
		printf("send yes\n");
	else
		printf("send no\n");	

	return;
}










































