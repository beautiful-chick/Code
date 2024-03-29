/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  socket_process_server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "20/02/24 12:24:15"
 *                 多进程服务器编写
 ********************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#define MAX_LISTEN 13

void help_printf(char *program);
int socket_init(int *listen_fd,int *listen_port);

int main(int argc,char *argv[])
{
	int                         choo = -1;
	int							listen_port = -1;
	int 						listen_fd = -1;
	int							client_fd = -1;
	int 						rv = -1;
	pid_t						pid = -1;
	struct sockaddr_in			clieaddr_in;
	socklen_t					clieaddr_len;
	struct option 				opts[]=
	{
		{"help",no_argument,NULL,'h'},
		{"port",required_argument,NULL,'p'},
		{0,0,0,0}
	};

  	while ((choo = getopt_long(argc,argv,"hp:",opts,NULL)) != -1)
	{
		switch(choo)
		{
			case 'h':
				help_printf(argv[0]);
				return 0;
			case 'p':
				listen_port = atoi(optarg);
				break;
		}
	}

	if (listen_port < 0)
	{
		help_printf(argv[0]);
		return 0;
	}

	printf("will listen port:%d\n",listen_port);

	//开始socket+listen初始化
	if ((rv = socket_init(&listen_fd,&listen_port)) < 0)
	{
		printf("socket error:%s\n",strerror(errno));
		return -1;
	}

	while(1)
	{
		printf("waiting for connect...\n");
	    memset(&clieaddr_in,0,sizeof(clieaddr_in));
		clieaddr_len = sizeof(clieaddr_in);
		if ( (client_fd = accept(listen_fd,(struct sockaddr*)&clieaddr_in,&clieaddr_len)) < 0 )
		{
			printf("accept error:%s\n",strerror(errno));
			continue;//accept若出错后跳过此次循环，进行下一次accept
		}
		printf("create client_fd:%d success!\n",client_fd);
		
		if ((pid = fork()) < 0)
		{
			printf("fork error:%s\n",strerror(errno));
			close(listen_fd);
			close(client_fd);
			return -2;
		}
		else if (pid > 0)//this is parents' process
		{
			close(client_fd);
			continue;//跳过本次循环，继续accept
		}
		else if (0 == pid)//this is child process
		{
			char 							buf[1024];//子进程独有的变量

			close(listen_fd);

			while(1)
			{
				rv = -1;
				memset(buf,0,sizeof(buf));
				if ((rv = read(client_fd,buf,sizeof(buf))) < 0)
				{
					close(client_fd);
					printf("read error:%s\n",strerror(errno));
					return -3;
				}
				else if ( 0 == rv )
				{
					printf("debug %s\n",buf );
					printf("client[%d] has disconnect\n",getpid());
					close(client_fd);
					return -4;
				}
				else if(rv > 0)
				{
					printf("client message:[%dbytes]:%s\n",rv,buf);
					if (write(client_fd,"connect success\n",sizeof("connect success\n")) < 0 )
					{
						printf("write error:%s\n",strerror(errno));
						return -5;
					}
					printf("sent message to client success!\n");
				}

			}	
		}
	}

	return 0;
}

//help菜单打印函数
void help_printf(char *program)
{
	printf("%s usage:\n",program);
	printf("--help(-h):printf help menu\n");
	printf("--port(-p):listen_port: 8999 ~ 20000\n");

	return ;
}

//socketinit函数，初始化socket——fd到listen阶段
int socket_init(int *listen_fd,int *listen_port)
{
	struct sockaddr_in					servaddr_in;
	int 								rv = -1;

	if ((*listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("socket error:%s\n",strerror(errno));
		return -1;
	}
	printf("listen_fd:%d\n",*listen_fd);
    
	memset(&servaddr_in,0,sizeof(servaddr_in));
	servaddr_in.sin_family   			= AF_INET;
	servaddr_in.sin_port 				= htons(*listen_port);
	servaddr_in.sin_addr.s_addr 		= htonl(INADDR_ANY);
	if ((rv = bind(*listen_fd,(struct sockaddr*)&servaddr_in,sizeof(servaddr_in))) < 0)
	{
		 printf("bind error:%s\n",strerror(errno));
		 close(*listen_fd);
		 return -2;
	}
	printf("bind success!\n");
     
	if ( (rv = listen(*listen_fd,MAX_LISTEN)) < 0 )
	{
		printf("listen error:%s\n",strerror(errno));
		close(*listen_fd);
		return -3;
	}
	printf("listen success!\n");

	printf("sock_init success listen_fd[%d]\n",*listen_fd);
	
	return 0;
}
