#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <sqlite3.h>

#include "socket_server.h"

int socket_server_init(char *listenip,int port)
{
	struct         sockaddr_in servaddr;
	int            rv=-1;
	int            on=1;
	int            listenfd=-1;

	listenfd=socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("Create socket failure: %s\n", strerror(errno));
		return -1;
	}
	printf("Create socket[%d] successfully!\n", listenfd);

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port = htons(port);

	if(listenip==NULL)
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		if(inet_pton(AF_INET,listenip,&servaddr.sin_addr)<0)
		{
			printf("Execute inet_pton failure:%s\n",strerror(errno));
			rv=-2;
			goto CleanUp;
		}
	}

	rv=bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(rv < 0)
	{
		printf("Socket[%d] bind on port[%d] failure: %s\n", listenfd, port, strerror(errno));
		rv=-3;
		goto CleanUp;
	}

	listen(listenfd, 50);
	rv=listenfd;
	goto CleanUp;

CleanUp:
	if(rv>0)
		return listenfd;
	else
		return rv;
}
