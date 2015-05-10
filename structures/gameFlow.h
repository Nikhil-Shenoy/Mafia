#include <stdlib.h>
#include <time.h>
#include "player.h"

void assignRoles(PlayerList *players);
void whoWillYouKill(PlayerList *players);
void whoWillYouSave(PlayerList *players);
void whoWillYouInvestigate(PlayerList *players);
void collectVotes(PlayerList *players);
void apply(void (*a)(), PlayerList *players);
void kill(Player *player);
