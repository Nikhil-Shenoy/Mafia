#ifndef GAMEFLOW_H
#define GAMEFLOW_H

#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "../chat/sock.h"

void assignRoles(PlayerList *players);
void describeRole(Player *p, void *aux);
void doAction(PlayerList *players, Role r);
void resetSaved(Player *p, void *aux);
void resetVote(Player *p, void *aux);
void printVotes(Player *p, void *aux);
bool townspeopleVictory(PlayerList *players);
bool mafiaVictory(PlayerList *players);

#endif
