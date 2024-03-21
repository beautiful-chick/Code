/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_pack.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(19/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "19/03/24 10:11:45"
 *                 
 ********************************************************************************/

#include "socket_pack.h"



int socket_init(char *server_ip,int port)
{
	struct sockaddr_in 			serv_addr;
	int    						socketfd;
	int 						flag6 = 0;
	if((socketfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("socket fd failure: %s\n",strerror(errno));
		return -1;
	}
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port    = htons(port);
	inet_aton(server_ip,&serv_addr.sin_addr);
	if((connect(socketfd,(struct  sockaddr *)&serv_addr,sizeof(serv_addr))) < 0)
	{
		printf("connect failure: %s\n",strerror(errno));
		flag6 = -1;
	}
	if(flag6 == -1)
	{
		return flag6;
	}
	else
	{
		return socketfd;
	}
}

int socket_write(int socketfd,char *buf,int bytes)
{
	int num = 0;
	num = write(socketfd,buf,bytes);
	return num;
}

int socket_read(int socketfd,char *buf,int bytes)
{
	int num = 0;
	num = read(socketfd,buf,bytes);

	return num;
}

