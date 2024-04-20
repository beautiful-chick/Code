/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  poll_server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(24/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "24/03/24 21:12:38"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <poll.h>

#define     ARRAY_SIZE(x)     (sizeof(x)/sizeof(x[0]))

static inline void print_usage(char *progname);
int  socket_server_init(char *listen_ip, int listen_port);

int main(int argc, char **argv)
{
	int   		   ch;
	char   		   *progname = NULL;
	int            daemon_run = 0;
	int            listenfd, connfd;
	int            serv_port = 0;
	struct pollfd  fds_array[1024];
	int            max;
	int            i, j;
	int            rv;
	int            found;
	char           buf[1024];

	struct  option opt[] = {
		{"daemon", no_argument, NULL, 'd'},
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};
	
	progname = basename(argv[0]);

	while( (ch = getopt_long(argc, argv, "dp:h", opt, NULL)) != -1 )
	{
		switch(ch)
		{
			case 'd':
				daemon_run = 1;
				break;
			case 'p':
				serv_port = atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;
		}
	}

	if( !serv_port )
	{
		print_usage(progname);
		return -1;
	}

	if( (listenfd = socket_server_init(NULL, serv_port)) < 0 )
	{
		printf("ERROR: %s server lsiten on port %d failure\n", argv[0], serv_port);
		return -2;
	}
	printf("%s server start to listen on port %d\n", argv[0], serv_port);

	if( daemon_run )
	{
		daemon(0, 0);
	}

	for(i=0; i<ARRAY_SIZE(fds_array); i++)
	{
		fds_array[i].fd = -1;
	}
	fds_array[0].fd = listenfd;
	fds_array[0].events = POLLIN;

	max = 0;

	for( ; ;)
	{
		/* program will blocked here */
		rv = poll(fds_array, max+1, -1);
		if(rv < 0)
		{
			printf("poll failure: %s\n", strerror(errno));
			break;
		}
		else if(rv == 0)
		{
			printf("poll get timeout\n");
			continue;
		}

		/*  listen socket get event means new client start connect now */
		if(fds_array[0].revents & POLLIN)
		{
			if( (connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0 )
			{
				printf("accpet new client failure: %s\n", strerror(errno));
				continue;
			}

			found = 0;
			for(i=0; i<ARRAY_SIZE(fds_array); i++)
			{
				if(fds_array[i].fd < 0)
				{
					printf("accept new client [%d] and add it into array\n", connfd);
					fds_array[i].fd = connfd;
					fds_array[i].events = POLLIN;
					found = 1;
					break;
				}
			}

			if( !found )
			{
				printf("accept new client [%d] but full, so refuse it\n", connfd);
				close(connfd);
				continue;
			}

			max = i>max ? i : max;
			if(--rv <=0 )
				continue;
		}
		else /* data arrive from already connected client */
		{
			for(i=0; i<ARRAY_SIZE(fds_array); i++)
			{
				if( fds_array[i].fd < 0 )
					continue;
				
				if( (rv = read(fds_array[i].fd, buf, sizeof(buf))) <= 0 )
				{
					printf("socket[%d] read failure or get disconnected.\n", fds_array[i]);
					close(fds_array[i].fd);
					fds_array[i].fd = -1;
				}
				else
				{
					printf("soxket[%d] read get %d bytes data\n", fds_array[i].fd, rv);
					for(j=0; j<rv; j++)
						buf[j] = toupper(buf[j]);

					if( write(fds_array[i].fd, buf, rv) < 0 )
					{
						printf("socket[%d] write failure: %s\n", fds_array[i].fd, strerror(errno));
						close(fds_array[i].fd);
						fds_array[i].fd = -1;
					}
				}
			}
		}
	}
CleanUp:
	close(listenfd);
	return 0;
}


static inline void print_usage(char *progname)
{
	printf("Usgae : %s [OPTION]...\n",progname);
	printf(" %s is a socket server proram, which used to verify client and echo back string from it\n",progname);
	printf(" -d [daemon] set program running on backgroud\n");
	printf(" -p [port] socket server port address\n");
	printf(" -h [help] Display this help information\n");

	printf("\nEXAMPLE:%s -d -p 8900\n", progname);
	return ;
}

int socket_server_init(char *listen_ip, int listen_port)
{
	struct sockaddr_in    servaddr;
	int                   listenfd = -1;
	int                   on = 1;
	int                   rv = 0;

	
	if( (listenfd = socket( AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		printf("Use socket() create a TCP socket failure: %s\n",strerror(errno));
		return -1;
	}
	//printf("Socket() [%d] success\n", listenfd);
	
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(listen_port);
	
	if( !listen_ip )
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		if(inet_pton(AF_INET, listen_ip, &servaddr.sin_addr) <= 0)
		{
			printf("inet_pton() set lsiten ip address failure.\n");
			rv = -2;
			goto CleanUp;
		}
	}
	
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind() to bind the TCP socket failure: %s\n",strerror(errno));
		rv = -3;
		goto CleanUp;
	}
	//printf("bind to THE TCP success listenfd[%d]\n",listenfd); 
	
	if(listen(listenfd, 13) < 0)
	{
		printf("listen() the port failure: %s\n",strerror(errno));
		rv = -4;
		goto CleanUp;
	}

CleanUp:
	if(rv < 0)
		close(listenfd);
	else
		rv = listenfd;

	return rv;
}




