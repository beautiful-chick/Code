/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 15:26:06"
 *                 
 ********************************************************************************/

#include "socket_server.h"
#include "time.h"
#include "stdarg.h"
#include "stdlib.h"
#include "sys/stat.h"
int socket_server_init(char *listen_ip, int listen_port)
{
	struct sockaddr_in 		 servaddr;
	int						 rv = 0;
	int 					 on = 1;
	int 					 listenfd;
	const char				 *filepath = "server_log.txt";
	FILE					 *file = fopen(filepath,"a+");
	time_t                   current_time;
	struct 				     tm *time_info;
	char					 timeString[25];
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		current_time = time(NULL); //获取当前时间戳
		time_info = localtime(&current_time);
		strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);
		fprintf(file,"Use socket() to create a TCP socket failure: %s -- time: %s\n", strerror(errno),timeString);
		return -1;
	}
	/*设置套接字端口可重用*/
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
		if (inet_pton(AF_INET, listen_ip, &servaddr.sin_addr) <= 0)
		{
        	current_time = time(NULL); //获取当前时间戳
		    time_info = localtime(&current_time);
			strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);
			fprintf(file,"inet_pton() set listen IP address failure -- time: %s\n",timeString);
			rv = -2;
			goto CleanUp;
		}
	}

	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
	{
		current_time = time(NULL); //获取当前时间戳
		time_info = localtime(&current_time);
		strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info);
		fprintf(file,"Use bind() to bind the TCP socket failure: %s -- time: %s\n",strerror(errno),timeString);
		rv = -3;
		goto CleanUp;
	}
	listen(listenfd,64);
CleanUp:
	if(rv < 0)
	{
		close(listenfd);
	}
	else
	{
		rv = listenfd;
	}

	return rv;
}

void set_socket_rlimit(void) //设置最大可·
{
	struct rlimit limit = {0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);
}
