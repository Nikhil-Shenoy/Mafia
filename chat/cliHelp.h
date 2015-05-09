#ifndef CLIHELP_H
#define CLIHELP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "vars.h"

struct cliPacket {
	char name[MAXLINE];
	char message[MAXLINE];
};

typedef struct cliPacket CliPacket;

void sendCliPacket(char *Name,char *Message, int sockfd, struct sockaddr_in *servaddr);

#endif
