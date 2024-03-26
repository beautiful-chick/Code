/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server_epoll.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(13/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "13/03/24 10:52:07"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sqlite3.h>
#include "sqliteall.h"

#define MAX_EVENTS       512
#define ARRAY_SIZE(x)    (sizeof(x)/sizeof(x[0]))

static inline void print_usage(char *progname);
int socket_server_init(char *listen_ip, int listen_port);
void set_socket_rlimit(void);

int main(int argc, char **argv)
{
	int                        listenfd, connfd;
	int                        serv_port = 0;
	int                        daemon_run = 0;
	char                      *progname = NULL;
	int                        opt;
	int                        rv;
	int                        i, j;
	int                        found;
	char                       buf[1024];
	char                       date[16];
	char                       time[16];
	char                       date_time[32];
	char                       id[16];
	float                      temperature;
	sqlite3                   *db;
	char                      *sql;

	int                        epollfd;
	struct epoll_event         event;
	struct epoll_event         event_array[MAX_EVENTS];
	int                        events;

	//open_or_create_sqlite(db);
	//create_statement(db, sql);

	struct option              long_options[] =
	{
		{"daemon", no_argument, NULL, 'b'},
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	progname = basename(argv[0]);
	while((opt = getopt_long(argc, argv, "bp:h", long_options, NULL)) != -1)
	{
		switch(opt)
		{
			case'b':
				daemon_run = 1;
				break;
			case'p':
				serv_port = atoi(optarg);
				break;
			case'h':
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

	create_statement(db, sql);


	set_socket_rlimit();

	if((listenfd = socket_server_init(NULL, serv_port)) < 0)
	{
		printf("ERROR: %s server listen on port %d failure: %s\n",argv[0],serv_port,strerror(errno));
		return -2;
	}
	printf("%s server start to listen on port %d\n",argv[0], serv_port);

	if(daemon_run)
	{
		daemon(0, 0);
	}

	if((epollfd = epoll_create(MAX_EVENTS)) < 0)
	{
		printf("epoll_create() failure: %s\n",strerror(errno));
		return -3;
	}

	event.events = EPOLLIN;
	event.data.fd = listenfd;

	if( epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) < 0)
	{
		printf("epoll add listen socket failure: %s\n", strerror(errno));
		return -4;
	}

	for( ; ; )
	{
		events = epoll_wait(epollfd, event_array, MAX_EVENTS, -1);
		if(events < 0)
		{
			printf("epoll failure: %s\n",strerror(errno));
			break;
		}
		else if(events==0)
		{
			printf("epoll get timeout\n");
			continue;
		}

		for(i=0; i<events; i++)
		{
			if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]: %s\n",event_array[i].data.fd, strerror(errno));
				epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
				close(event_array[i].data.fd);
			}

			if(event_array[i].data.fd == listenfd)
			{
				if((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0)
				{
					printf("accept new client failure: %s\n", strerror(errno));
					continue;
				}

				event.data.fd = connfd;
				event.events = EPOLLIN;
				if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) < 0)
				{
					printf("epoll add client socket failure: %s\n", strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new socket[%d] ok.\n",connfd);
			}
			else
			{
				if((rv=read(event_array[i].data.fd, buf, sizeof(buf))) <= 0)
				{
					printf("socket[%d] read failure or get disconncet and will be removed.\n",
							event_array[i].data.fd);
					epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{
					printf("\n");
					printf("socket[%d] read get %d bytes data:%sºC\n", event_array[i].data.fd, rv,buf);
					sscanf(buf, "%s %s %s %f", id, date, time, &temperature);
					memset(date_time, 0, sizeof(date_time));
					snprintf(date_time, sizeof(date_time), "%s %s", date, time);

					insert_statement(db, sql, id, date_time, &temperature);

					//printf("%s %s %.4f\n",id, date_time,  temperature);
					/*
					   for(j=0; j<rv; j++)
					   {
					   buf[j]=toupper(buf[j]);
					   }
					   */
					if(write(event_array[i].data.fd, buf, rv) < 0)
					{
						printf("socket[%d] write failure: %s\n",event_array[i].data.fd, strerror(errno));
						epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
						close(event_array[i].data.fd);
					}
				}
			}
		}
	}
ClenUp:
	close(listenfd);
	return 0;
}

static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n",progname);

	printf(" %s is a socket server program, which used to verify client and echo back string from it\n",progname);

	printf("\nMandatroy arguments to long options are mandatroy for short options too:\n");

	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port ] Socket server port address\n");
	printf(" _h[help ] Display this help information\n");

	printf("\nExample: %s -b -p 8900\n",progname);

	return ;
}

int socket_server_init(char *listen_ip, int listen_port)
{
	struct sockaddr_in         servaddr;
	int                        rv = 0;
	int                        on = 1;
	int                        listenfd;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{

		printf("Use socket create a TCP socket failure: %s\n", strerror(errno));
		return -1;
	}

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
			printf("inet_pton() set listen IP address failure.\n");
			rv = -2;
			goto CleanUp;
		}
	}

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
		rv = -3;
		goto CleanUp;
	}

	if(listen(listenfd, 64) < 0)
	{
		printf("Use listen() to listen the TCP socket failure: %s\n", strerror(errno));
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

void set_socket_rlimit(void)
{
	struct rlimit limit = {0};

	getrlimit(RLIMIT_NOFILE, &limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE, &limit);

	printf("set socket open fd max count to %ld\n", limit.rlim_max);
}

