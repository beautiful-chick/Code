/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server_epoll.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 16:05:44"
 *                 
 ********************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "errno.h"
#include "sys/epoll.h"
#include "sys/resource.h"
#include "socket_server.h"
#include "getopt.h"
#include "unistd.h"
#include "libgen.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "sqlite3.h"
#include "stdarg.h"
#include "time.h"
#define MAX_EVENTS		512
#define ARRAY_SIZE(x)	(sizeof(x0)/sizeof(x[0]))
#define TABLE			"my_temp_lib"

time_t					current_time;
struct					tm *time_info;
char					timeString[25];
const char              *filepath = "../server_lib/server_log.txt";
//FILE                    *file = fopen(filepath,"a+");

static inline void print_usage(char *progname);
void  			   server_lib(char *buf);
int main(int argc,char **argv)
{
		int 				listenfd,connfd;
		int 				serv_port = 0;
		int					daemon_run = 0;
		char 				*progname = NULL;
		int 				opt;
		int 				rv;
		int 				i,j;
		int 				found;
		char 				buf[100];
		int					epollfd;
		struct				epoll_event event;
		struct				epoll_event	event_array[MAX_EVENTS];
		int 				events;
		FILE				*file = fopen(filepath,"a+");		

		struct option long_options[]=
		{
			{"daemon",no_argument,NULL,'b'},
			{"port",required_argument,NULL,'p'},
			{"help",no_argument,NULL,'h'},
			{NULL,0,NULL,0}
		};

		progname = basename(argv[0]);

		while((opt = getopt_long(argc,argv,"bp:h",long_options,NULL))!=-1)
		{
			switch(opt)
			{
				case 'b':
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

		if(!serv_port)
		{
			print_usage(progname);
			return -1;
		}

		set_socket_rlimit();

		if((listenfd=socket_server_init(NULL,serv_port)) < 0)
		{	
			current_time = time(NULL); //获取当前时间戳
			time_info = localtime(&current_time);
			strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);
			fprintf(file,"ERROR: %s server listen on port %d failure -- time: %s\n",argv[0],serv_port,timeString);
			fflush(file);
			return -2;
		}
		printf("%s server start to listen on port %d\n",argv[0],serv_port);
		printf("socket:%d\n",listenfd);
		if(daemon_run)
		{
			daemon(0,0);
		}

		if((epollfd = epoll_create(MAX_EVENTS)) < 0)
		{
			current_time = time(NULL); //获取当前时间戳
			time_info = localtime(&current_time);
			strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);
			fprintf(file,"epoll_create() failure: %s -- time: %s\n",strerror(errno),timeString);
			fflush(file);
			return -3;
		}

		event.events = EPOLLIN;
		event.data.fd = listenfd;

		if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event) < 0)
		{
            current_time = time(NULL); //获取当前时间戳
			time_info = localtime(&current_time);
			strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);
			fprintf(file,"epoll add listen socket failure: %s\n",strerror(errno),timeString);
			fflush(file);
			return -4;
		}
		printf("event.data.fd:%d\n",event.data.fd);
	//	sprintf(sql,"CREATE TABLE IF NOT EXISTS " TABLE "(TEMP TEXT NOT NULL);");
	//	sqlite3_exec(db,sql,0,0,0);
		for(;;)
		{
        	current_time = time(NULL); //获取当前时间戳
			time_info = localtime(&current_time);
			strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);

			events = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);

			if(events < 0)
			{
				printf("%d\n",events);
				fprintf(file,"epoll failure:%s -- time: %s\n",strerror(errno),timeString);
				fflush(file);
				break;
			}
			else if(events == 0)
			{
				fprintf(file,"epoll get timeout -- time: %s\n",timeString);
				fflush(file);
				continue;
			}
			for(int i=0; i<events; i++)
			{
				if ( (event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP) )
				{

					fprintf(file,"epoll_wait get error on fd[%d]: %s -- time: %s\n", event_array[i].data.fd, strerror(errno),timeString);
					fflush(file);
					close(event_array[i].data.fd);
				}

				if(event_array[i].data.fd == listenfd)
				{
			
					if((connfd = accept(listenfd,(struct sockaddr *)NULL,NULL)) < 0)
					{
						fprintf(file,"accept new socket: %s -- time: %s\n",strerror(errno),timeString);
						fflush(file);
						return -2;
					}


					event.data.fd = connfd;
					event.events = EPOLLIN;

					if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event) < 0)
					{
						fprintf(file,"epoll add client socket failure: %s -- time: %s\n",strerror(errno),timeString);
						fflush(file);
						close(event_array[i].data.fd);
						continue;
					}

					printf("epoll add new clinet socket[%d] ok\n",connfd);
				}

				else
				{
					if((rv = read(event_array[i].data.fd,buf,sizeof(buf))) <= 0)
					{
						printf("rv = %d\n",rv);
						fprintf(file,"socket[%d] read failure:%s -- time: %s\n",event_array[i].data.fd,strerror(errno),timeString);
						fflush(file);
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
						continue;
					}

					if(rv > 0)
					{
						server_lib(buf);
					}

					printf("%s\n",buf);
				}
			}
		}
CleanUp:
		close(listenfd);
		return 0;
}

static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTIN].. \n",progname);

	printf(" %s is a socket server program, which used to verify client and echo back string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");

	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port ] Socket server port address\n");
	printf(" -h[help ] Display this help information\n");

	printf("\nExample: %s -b -p 8900\n", progname);
	return ;
}

void server_lib(char *buf)
{
	char    buff[100] = "../server_lib/server_data_lib.db";	
	int		rc;
	char 	sql[256];
	sqlite3 *db;
	//sprintf(sql,"CREATE TABLE IF NOT EXISTS " TABLE "(TEMP TEXT NOT NULL);");
	//sqlite3_exec(db,sql,0,0,0);
	if(sqlite3_open(buff,&db) != SQLITE_OK)
	{
		printf("sqlite3_open failure: %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}

	sprintf(sql,"CREATE TABLE IF NOT EXISTS " TABLE "(TEMP TEXT NOT NULL);");
	sqlite3_exec(db,sql,0,0,0);
	memset(sql,0,sizeof(sql));

	sprintf(sql,"insert into %s values('%s');",TABLE,buf);
	if(sqlite3_exec(db,sql,0,0,0) != SQLITE_OK)
	{
		printf("sqlite3_exec failure: %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	
	
}
