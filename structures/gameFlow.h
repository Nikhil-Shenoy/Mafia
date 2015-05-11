#ifndef GAMEFLOW_H
#define GAMEFLOW_H

#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "../chat/sock.h"

void assignRoles(PlayerList *players);
void describeRole(Player *p, void *aux);
void doAction(PlayerList *players, Role r);
void collectVotes(PlayerList *players);
void resetSaved(Player *p, void *aux);
void resetVote(Player *p, void *aux);
void printVotes(Player *p, void *aux);

#endif
