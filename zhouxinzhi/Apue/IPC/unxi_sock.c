/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  unxi_sock.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(03/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "03/03/24 20:32:55"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/socket.domain"

int main(int argc,char* argv[])
{
	int						rv = -1;
	int						listen_fd = -1;
	int						client_fd = -1;
	struct sockaddr_un		serv_addr;
	struct sockaddr_un		clie_addr;
	socklen_t				clieaddr_len;
	char					buf[1024];

	//socket process
	if ((listen_fd = socket(AF_LOCAL,SOCK_STREAM,0)) < 0)
	{
		printf("unix_sock error:%s\n",strerror(errno));
		return -1;
	}
	printf("socket create fd:%d\n",listen_fd);
	
	//check SOCK_PATH
	if ( !access(SOCK_PATH,F_OK) )
	{
		printf("SOCK_PATH:%s failture\n",SOCK_PATH);
		unlink(SOCK_PATH);
	}
	printf("SOCK_PATH:%s SUCCESS\n",SOCK_PATH);

	//bind start
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sun_family   = AF_LOCAL;
	strncpy(serv_addr.sun_path,SOCK_PATH,sizeof(serv_addr.sun_path)-1);

	if (bind(listen_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("bind error:%s\n",strerror(errno));
		unlink(SOCK_PATH);
		return -2;
	}
	printf("socket %d bind on path %s ok\n",listen_fd,SOCK_PATH);

	listen(listen_fd,13);
	
	while(1)
	{
		printf("start waiting for connect\n");
		clieaddr_len = sizeof(clie_addr);
		if ((client_fd=accept(listen_fd,(struct sockaddr*)&clie_addr,&clieaddr_len)) < 0)
		{
			printf("accept error:%s\n",strerror(errno));
			continue;
		}

		while(1)
		{
			sleep(3);
			memset(buf,0,sizeof(buf));
			rv = read(client_fd,buf,sizeof(buf));

			if (rv < 0)
			{
				printf("read error%s\n",strerror(errno));
				close(client_fd);
				continue;
			}
			else if (rv == 0)
			{
				printf("client fd has disconnect\n");
				close(client_fd);
				break;
			}
			else if (rv > 0)
			{
				printf("read %d bytes from client:%s\n",client_fd,buf);
	
				if (write(client_fd,"RECIVE SUCCESS\n",sizeof("RECIVE SUCCESS\n")) < 0)
				{
					printf("write error:%s\n",strerror(errno));
					close(client_fd);
					continue;
				}
			}	
		}
	}

	close(listen_fd);
}


