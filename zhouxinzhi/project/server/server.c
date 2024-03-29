/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(24/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "24/03/24 10:45:10"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <getopt.h>
#include "debug.h"
#include "socket.h"
#include "temp.h"
#include "string_handle.h"
#include "database.h"

#define MAX_EVENTS 256
#define DATABASE_NAME "server.db"
#define TABLE_NAME "server"
int printf_help(char *program_name);


int main(int argc,char *argv[])
{
	int							choo = -1;
	int 						daemon_state = 0;
	int 						server_port = -1;
	int 						i = -1;
	int							epoll_fd = -1;
	struct epoll_event			event;
	struct epoll_event          event_array[MAX_EVENTS];
	int							events;
	char						*server_ip = NULL;
	int 						listen_fd = -1;
	int 						client_fd = -1;
	int 						rv = -1;
	char						buf[128];
	t_temp_measure				recive_data;
	sqlite3						*db = NULL;
	struct option				opts[]=
	{
		{"daemon",required_argument,NULL,'d'},
		{"port",required_argument,NULL,'P'},
		{"ip",required_argument,NULL,'i'},
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	while ( (choo = getopt_long(argc,argv,"d:p:i:h",opts,NULL)) != -1)
	{
		switch (choo)
		{
			case 'd':
				daemon_state = atoi(optarg);
				break;
			case 'p':
				server_port = atoi(optarg);
				break;
			case 'i':
				server_ip = optarg;
				break;
			case 'h':
				printf_help(argv[0]);
				return 0;
		}

	}
	
	if ( (server_port <= 1024) )
	{
		printf_help(argv[0]);
		return -1;
	}
	printf("server_ip:%s server_port:%d\n",server_ip,server_port);

	if ( daemon_state == 1 )
	{
		printf("program will daemon use\n");
		daemon(0,0);
	}

	 //创建数据库以及表
	debug_printf("starting to creat db and table\n");
	
	if ( (db = sqlite3_open_database(DATABASE_NAME)) == NULL )
	{   
		printf("creat db error:%s\n",strerror(errno));
		return -2; 
	}   
	else
	{   
		debug_printf("create db success!\n");
		if ( sqlite3_create_table(db,TABLE_NAME) < 0 ) 
		{   
			printf("create table error:%s\n",strerror(errno));
			return -2;
		}
			debug_printf("create table success!\n");
	}   

	if ( (listen_fd = socket_listen_init(server_ip,server_port)) < 0 )
	{
		printf("create listen_fd for server error:%s\n",strerror(errno));
		return  -1;
	}
	debug_printf("listen_fd:%d create success\n",listen_fd);

	if ( (epoll_fd = epoll_create(MAX_EVENTS)) < 0)
	{
		printf("creat epoll_fd error:%s\n",strerror(errno));
		close(listen_fd);
		return -2;
	}

	event.events = EPOLLIN;
	event.data.fd = listen_fd;

	if ( epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&event) < 0 )
	{
		printf("epoll add listen socket failure:%s\n",strerror(errno));
		return -3;
	}

	while (1)
	{
		events = epoll_wait(epoll_fd,event_array,MAX_EVENTS,-1);
		if (events < 0)
		{
			printf("epoll failure:%s\n",strerror(errno));
			break;
		}
		else if (events == 0)
		{
			printf("epoll get timeout!\n");
			continue;
		}

		for (i=0; i<events; i++)
		{
			if ( (event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP) )
			{
				printf("epoll_wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
			}

			//若相应的是listen_fd，处理
			if (event_array[i].data.fd == listen_fd)
			{
				if ( (client_fd = accept(listen_fd,NULL,NULL)) < 0 ) 
				{
					printf("accept new client_fd error:%d\n",strerror(errno));
					continue;
				}

				event.data.fd = client_fd;
				event.events = EPOLLIN;

				if ( epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&event) < 0 )
				{
					printf("epoll add client socket error:%s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new client_fd : %d success\n",event_array[i].data.fd);
			}
			else   //若是客户端的请求，处理
			{
				if ( (rv = read(event_array[i].data.fd,buf,128)) < 0)
				{
					printf("read message from client failure :%s\n",strerror(errno));
					epoll_ctl(epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else if (rv == 0)
				{
					printf("client[%d] has disconnect!\n",event_array[i].data.fd);
					epoll_ctl(epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else if (rv > 0)
				{
					printf("client{%d} message:%s\n",event_array[i].data.fd,buf);
					
					apart_string(buf,"|",&recive_data);

					printf("%s\n",recive_data.dev_name);
					printf("%s\n",recive_data.ptime);
					printf("%.2f\n",recive_data.temp);

					sqlite3_insert_single(db,TABLE_NAME,&recive_data);
				
					continue;
				}
			}
		}
	}

	close(listen_fd);
	return 0;
}
 
int printf_help(char *program_name)
{
	printf("%s usage:\n",program_name);
	printf("--daemon(-d): set program use background with option 1\n");
	printf("--port(-p): use -p to set server port\n");
	printf("--ip(-i): use -i to set listen ip for server\n");
	printf("--help(-h): use -h to get help_menu\n");
	return 0;
}
