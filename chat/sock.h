#ifndef SOCK_H
#define SOCK_H

#include <unistd.h>
#include <arpa/inet.h>
#include "dbg.h"

int open_listenfd(int port);
ssize_t robustSend(int fd, void *usrbuf, size_t n);
int loggedAccept(int listenfd);

typedef struct sockaddr SA;

#endif
