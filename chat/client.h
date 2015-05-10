#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "dbg.h"
#include "vars.h"

typedef struct _cliPacket {
	char name[MAXLINE];
	char message[MAXLINE];
} CliPacket;

void sendCliPacket(char *Name,char *Message, int sockfd, struct sockaddr_in *servaddr);

#endif
