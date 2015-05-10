#include "client.h"

int main(int argc, char *argv[]) {

	if(argc != 2) {
		fprintf(stderr,"Usage: ./client <IP address of server>\n");
		exit(1);
	}

	int sockfd;
	struct sockaddr_in servaddr;

	char *host = argv[1];

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_aton(host,&servaddr.sin_addr);

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// Create socket to receive broadcasts
	struct sockaddr_in broadaddr;
	socklen_t sinlen = sizeof(struct sockaddr_in);

	int broadSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	broadaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	broadaddr.sin_port = htons(BROADCAST_PORT);
	broadaddr.sin_family = AF_INET;

	int status;
	status = bind(broadSock, (struct sockaddr *)&broadaddr, sinlen);
	debug("Bind Status for broadSock: %d", status);

	status = getsockname(broadSock, (struct sockaddr *)&broadaddr, &sinlen);
	debug("broadSock port: %d",htons(broadaddr.sin_port));

	uint32_t bytesRead;
	char mesg[MAXLINE];
	char recvline[2*MAXLINE];
	char name[MAXLINE];
	char statString[MAXLINE];

	bool chatDone = false;
	bool serverAccepts = true;

	printf("Starting the client...\n");

	printf("What is your name? ");
	fgets(name,MAXLINE,stdin);

	// Get rid of the newline at the end of the name
	name[strlen(name)-1] = '\0';

	while(!chatDone) {

		socklen_t length = sizeof(servaddr);

		while(serverAccepts) {
			printf("--> ");
			fgets(mesg,MAXLINE,stdin);
			sendCliPacket(name,mesg,sockfd,&servaddr);

			//bytesRead = recvfrom(BROADCAST_PORT,recvline,2*MAXLINE,0,(struct sockaddr *)&servaddr,&length);
		
			while(!isPacketMine(name,recvline,statString)) 
				bytesRead = recvfrom(BROADCAST_PORT,recvline,2*MAXLINE,0,(struct sockaddr *)&servaddr,&length);


			if(!streq(statString,"accepting"))
				serverAccepts = false;
			else
				printf("Received %u bytes.\n"
				       "Response is %s\n\n",
				       bytesRead,statString);
		}

		while(!streq(recvline,"end stream")) {
			bytesRead = recvfrom(broadSock,recvline,2*MAXLINE+10,0,(struct sockaddr *)&servaddr,&length);
			if(!streq(recvline,"end stream")) {
				printf("%s\n",recvline);
				memset(recvline,'\0',2*MAXLINE);
			}
		}

		memset(recvline,'\0',2*MAXLINE);

		bytesRead = recvfrom(broadSock,recvline,2*MAXLINE,0,(struct sockaddr *)&servaddr,&length);
		if(streq(recvline,"accepting"))
			serverAccepts = true;
	}

	return 0;
}

void sendCliPacket(char *Name,char *Message, int sockfd, struct sockaddr_in *servaddr) {
	// Create the client packet
	CliPacket myMessage;
	memset(&myMessage,'\0',sizeof(myMessage));

	// Construct the packet
	strncpy(myMessage.name,Name,strlen(Name));
	strncpy(myMessage.message,Message,strlen(Message));

	sendto(sockfd,(const void *)&myMessage,sizeof(myMessage),0,(struct sockaddr *)servaddr,sizeof(*servaddr));
}
