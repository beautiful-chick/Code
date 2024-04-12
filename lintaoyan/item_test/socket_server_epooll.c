/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_server_epooll.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "11/04/24 08:06:00"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>

#define	MAX_EVENTS		512
#define	ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))

static inline void display_usage(char *progname);
void set_socket_rlimit(void);
int socket_server_init(char *listen_ip, int listen_port);

int main (int argc, char **argv)
{
	int							rv;
	int							ch;
	int							ser_port;
	int							daemon_run = 0;
	char					   *progname = NULL;
	int							listen_fd, conn_fd;
	char						buf[1024];
	int							i, j;
	int							found;

	int							epoll_fd;
	struct epoll_event			event;
	struct epoll_event          event_array[MAX_EVENTS];
	int							events;

	struct option  				long_options[] = {
		{"daemon", no_argument, NULL, 'd'},
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{"NULL, 0, NULL, 0"}
	};

	progname = basename(argv[0]);

	while( (ch = getopt_long(argc, argv, "bp:h", long_options, NULL)) != -1)
	{
		switch(ch)
		{
			case 'd':
				daemon_run = 1;
				break;
			case 'p':
				ser_port = atoi(optarg);
				break;
			case 'h':
				display_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;

		}
	}

	if(!ser_port)
	{
		display_usage(progname);
		return -1;
	}

	set_socket_rlimit();

	if( (listen_fd = socket_server_init(NULL, ser_port)) < 0 )
	{
		printf("Error: %s server listen on port %d failure\n", argv[0], ser_port);
		return -2;
	}
	printf ("%s server start to listen on port[%d]\n", argv[0], ser_port);

	if(daemon_run)
	{
		daemon(0, 0);
	}

	if( (epoll_fd = epoll_create(MAX_EVENTS)) < 0 )
	{

		printf ("epoll_create() failure: %s\n", strerror(errno));
		return -3;
	}

	event.events = EPOLLIN;
	event.data.fd = listen_fd;

	if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) < 0)
	{
		printf("epoll add listen socket failure: %s\n", strerror(errno));
		return -4;
	}

	while(1)
	{
		events = epoll_wait(epoll_fd, event_array, MAX_EVENTS, -1);
		if(events < 0)
		{
			printf("epoll_wait failure: %s\n", strerror(errno));
			break;
		}
		else if(events == 0)
		{
			printf("epoll timeout. \n");
			continue;
		}

		for(i=0; i<events; i++)
		{
			if( (event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP) )
			{
				printf("epoll_wait erroron fd[%d]: %s\n", event_array[i].data.fd, strerror(errno));
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
				close(event_array[i].data.fd);
			}

			if(event_array[i].data.fd == listen_fd)
			{
				if( (conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL)) < 0 )
				{
					printf("accept new client failure: %s\n", strerror(errno));
					continue;
				}

				event.data.fd = conn_fd;
				event.events = EPOLLIN;
				if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event) < 0)
				{
					printf("epoll add client socket failure: %s\n", strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new client socket[%d] ok.\n", conn_fd);
			}
			else
			{
				if( (rv = read(event_array[i].data.fd, buf, sizeof(buf))) <= 0 )
				{
					printf("read() failure or get disconnected \n", event_array[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{
					printf("socket[%d] read %d  bytes data from client: %s\n",event_array[i].data.fd,  rv, buf);

					for(j=0; j<rv; j++)
						buf[j] = toupper(buf[j]);

					if( write(event_array[i].data.fd, buf, rv)< 0 )
					{
						printf("socket[%d] write back to client failure: %s\n", event_array[i].data.fd,  strerror(errno));
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
						close(event_array[i].data.fd); 
					}
				}
			}

		}
	}
cleanup:
	close(listen_fd);
	return 0;
} 

static inline void display_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n", progname);
	printf("%s is a socket server program.\n", progname);

	printf(" -d[--daemon] set program running on background\n");
	printf(" -p[--port] socket server port address\n");
	printf(" -h[--help] Display help informations\n ");

	return ;
}


void set_socket_rlimit(void)
{
	struct	rlimit	limit = {0};

	getrlimit(RLIMIT_NOFILE, &limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE, &limit);

	printf(" set socket open fd max count to %d\n", limit.rlim_max);
}

int socket_server_init(char *listen_ip, int listen_port)
{
	struct sockaddr_in				ser_addr;
	int								rv = 0;
	int								on = 1;
	int								listen_fd;

	if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("socket() create listen_fd failure: %s\n", strerror(errno));
		return -1;
	}

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(listen_port);

	if(!listen_ip)
	{
		ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else 
	{
		if(inet_pton(AF_INET, listen_ip, &ser_addr.sin_addr) <= 0)
		{
			printf("inet_pton() set listen_ip failiure: %s\n", strerror(errno));
			rv = -2;
			goto cleanup;
		}
	}

	if( bind(listen_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0)
	{
		printf ("bind() failure: %s\n", strerror(errno));
		rv = -3;
		goto cleanup;
	}

	if(listen(listen_fd, 64) < 0)
	{
		printf ("listen() failure:%s\n", strerror(errno));
		rv = -4;
		goto cleanup;
	}

cleanup:
	if(rv < 0)
		close(listen_fd);
	else
		rv = listen_fd;
	return rv;
}



















