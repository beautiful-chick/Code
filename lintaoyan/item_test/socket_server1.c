/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_server1.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 19:05:24"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>

void display_usage(char *progname)
{

	printf ("%s usage: \n", progname);
	printf ("-p(--port): specify server listen port.\n");
	printf ("-h(--help): print this help information.\n");

	return ;
}


int main (int argc, char **argv)
{
	int						 sock_fd, cli_fd;
	int						 rv;
	int						 ch;
	int						 port;
	struct sockaddr_in       ser_addr;
	struct sockaddr_in       cli_addr;
	socklen_t                cli_addrlen;
	char                     buf[1024];
	int 					 on =1;
	struct option                   opts[] = {
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};



	while( (ch = getopt_long(argc, argv, "p:h", opts, NULL)) != -1 )
	{
		switch(ch)
		{
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				display_usage(argv[0]);
				return 0;
			default:
				display_usage(argv[0]);
				return 1;
		}
	}

	if( !port )
	{
		display_usage(argv[0]);
		return 0;
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0)
	{
			printf("Create socket failure: %s\n", strerror(errno));
			return -1;
	}
	printf("Create socket[%d] successfully!\n", sock_fd);

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	rv = bind(sock_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(rv < 0)
	{
		printf("Socket[%d] bind on port[%d] failure: %s\n", sock_fd, port, strerror(errno));
		return -2;
	}

	listen(sock_fd, 13);
	printf ("Start to listen on port[%d] \n", port);

	cli_addrlen = sizeof(cli_addr);
	
/*  	
	printf ("Start accept new client incoming...\n");
	cli_fd  = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addrlen);
	if(cli_fd < 0)
	{
		printf("Accept new client failure: %s\n", strerror(errno));
	}
	printf("Accept new client[%s:%d] successfully\n", inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
*/
	while(1)
	{
  		
  		printf ("Start accept new client incoming...\n");
		cli_fd  = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addrlen);
		if(cli_fd < 0)
		{
			printf("Accept new client failure: %s\n", strerror(errno));
			continue;
		}
		printf("Accept new client[%s:%d] successfully\n", inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));

		while(1)
		{
		memset(buf, 0, sizeof(buf));
		rv = read(cli_fd, buf, sizeof(buf));
		if(rv < 0)
		{
			printf("Read data from client socket[%d] failure: %s\n", cli_fd, strerror(errno));
			close(cli_fd);
			continue;
		}
		else if(rv == 0)
		{
         	printf("client socket[%d] disconnected\n", cli_fd);
			close(cli_fd);
			continue;
		}
		else if(rv > 0)
		{
			printf("read %d bytes data from client[%d] and echo it back: '%s'\n", rv, cli_fd, buf);
		}

		rv = write(cli_fd, buf, rv);
		if(rv < 0)
		{
			printf("Write %d bytes data back to client[%d] failure: %s\n", rv, cli_fd, strerror(errno));
			close(cli_fd);
		}
		}	
	}
    close(sock_fd);
    return 0;
}

