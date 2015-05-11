#include "util.h"

int fdloop(fd_set *fdset, int fdmax, int listenfd,
		   loopExpr listenExpr, loopExpr clientExpr, void *aux)
{
	fd_set fdset_copy;
	FD_ZERO(&fdset_copy);
	fdset_copy = *fdset;
	check((select(fdmax+1, &fdset_copy, NULL, NULL, NULL) != -1),
		  "select");
	for(int i = 0; i <= fdmax; i++) {
		if(FD_ISSET(i, &fdset_copy)) {
			int newfdmax = 0;
			if (i == listenfd) {
				newfdmax = listenExpr(fdset, i, aux);
			} else {
				newfdmax = clientExpr(fdset, i, aux);
			}
			fdmax = max(fdmax, newfdmax);
		}
	}
	return fdmax;
error:
	return -1;
}
