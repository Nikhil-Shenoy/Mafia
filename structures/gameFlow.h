#ifndef GAMEFLOW_H
#define GAMEFLOW_H

#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "../chat/sock.h"

void assignRoles(PlayerList *players);
void describeRole(Player *p, void *aux);
void whoWillYouKill(PlayerList *players);
void whoWillYouSave(PlayerList *players);
void whoWillYouInvestigate(PlayerList *players);
void collectVotes(PlayerList *players);
void apply(void (*a)(), PlayerList *players);
void kill(Player *player);

#endif
