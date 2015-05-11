#include "sock.h"

// STOLEN FROM CSAPP
/*
 * open_listenfd - open and return a listening socket on port
 *     Returns -1 and sets errno on Unix error.
 */
/* $begin open_listenfd */
int open_listenfd(int port)
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;

    /* Create a socket descriptor */
	check(((listenfd = socket(AF_INET, SOCK_STREAM, 0)) >= 0),
		  "socket() failed");

    // Eliminates "Address already in use" error from bind.
	// We don't want to set this on ephemeral ports, though.
	if (port > 0)
		check((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
						  (const void *)&optval , sizeof(int)) >= 0),
			  "setsockopt() failed");

    /* Listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    check((bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) >= 0),
		  "bind() failed");

    /* Make it a listening socket ready to accept connection requests */
    check((listen(listenfd, 1024) >= 0), "listen() failed");
    return listenfd;
error:
	return -1;
}

ssize_t robustSend(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nwritten = send(fd, bufp, nleft, MSG_NOSIGNAL)) <= 0) {
	    check(errno == EINTR, "send");
		nwritten = 0; // sigint, restart.
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
error:
	return (errno == EPIPE)? -2 : -1;
}


char ipbuf[INET6_ADDRSTRLEN];
const char *ipString(struct sockaddr_storage *s_addr) {
	SA *remoteaddr = (SA *)s_addr;
	return inet_ntop(remoteaddr->sa_family,
					 (remoteaddr->sa_family == AF_INET)?
					 (void *)&((struct sockaddr_in*)remoteaddr)->sin_addr :
					 (void *)&((struct sockaddr_in6*)remoteaddr)->sin6_addr,
					 ipbuf, INET6_ADDRSTRLEN);
}

int loggedAccept(int listenfd) {
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen = sizeof(remoteaddr);
	int newfd = accept(listenfd,
					   (struct sockaddr *)&remoteaddr,
					   &addrlen);

	check((newfd != -1), "accept");

	log_info("new connection from %s on socket %d",
			 ipString(&remoteaddr), newfd);
	return newfd;
error:
	perror("accept");
	return -1;
}

ssize_t trimmedRecvb(int fd, char *bufp, size_t n)
{
    size_t nleft = n;
    ssize_t nread;

    do {
		if ((nread = recv(fd, bufp, nleft, 0)) < 0) {
			check((errno == EINTR), "recv");
			nread = 0; // start over and call read() again
		} else if (nread == 0)
			break; // Done
		nleft -= nread;
		bufp += nread;
    } while (nleft > 0);

	ssize_t length = n - nleft;
	if (length >= 2) {
		bufp[length - 2] = '\0';
		return length - 1;
	}

    return length;
error:
	perror("recv");
	return -1;
}


ssize_t trimmedRecv(int fd, char *bufp, size_t n)
{
    size_t nleft = n;
    ssize_t nread;

	if ((nread = recv(fd, bufp, nleft, 0)) < 0) {
		check((errno == EINTR), "recv");
		nread = 0;
	}
	nleft -= nread;
	bufp += nread;

	ssize_t length = n - nleft;
	if (length >= 2) {
		bufp[-2] = '\0';
		return length - 1;
	}

    return length;
error:
	perror("recv");
	return -1;
}
