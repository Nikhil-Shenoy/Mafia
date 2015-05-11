#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include "dbg.h"

#define MAXLINE 2048
#define PORT 5000
#define BROADCAST_PORT 5001
#define PLAYERS 5
#define MSGCOUNT 5
#define READY_THRESHOLD 5

#define streq(A, B) (strcmp(A, B) == 0)
#define max(A,B) A < B? B: A

typedef int (*loopExpr)(fd_set *fdset, int fd, void *aux);

int fdloop(fd_set *fdset, int fdmax, int listenfd,
		   loopExpr listenExpr, loopExpr clientExpr, void *aux);

#endif
