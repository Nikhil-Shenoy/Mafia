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















