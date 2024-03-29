/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  socket_server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(18/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "18/02/24 11:44:41"
 *             
 *                  实现socket_server服务器，并且每隔接收一次温度
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>    
#include <sys/stat.h>       
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <arpa/inet.h>

#define MAX_LISTEN 13
#define LISTEN_PORT 8999

int help_printf( char *program );//打印使用信息
int socket_init(int *listen_fd,int listen_port);//进行socket初始化

int main( int argc,char *argv[])
{
	int 							listen_fd = -1;
	int 							client_fd = -1;
	int 							choo = -1;
	int								daemon_var = 0;//用来决定是否后台运行
	int                             listen_port = LISTEN_PORT;
	int 							rv =-1;
	char							buf[1024];
	struct option			 		opts[] = 
	{
		{"help",no_argument,NULL,'h'},
		{"port",required_argument,NULL,'p'},
		{"daemon",required_argument,NULL,'d'},
        {0,0,0,0}
	};
 
	//选项系统
	while((choo = getopt_long(argc,argv,"hp:d:",opts,NULL)) != -1)
	{
		switch(choo)
		{
			case 'h':
				help_printf(argv[0]);
				return 0;
				
			case 'p':
				listen_port = atoi(optarg);//optarg 不是值，而是指向选项后参数的指针
				break;

			case 'd':
				daemon_var = atoi(optarg);
				break;
		}
	}
	
	if (listen_port == LISTEN_PORT) 
	{
		printf("server will listen default port:%d\n",listen_port);
	}
	else
	{
		printf("server will listen port:%d\n",listen_port);
	}

    //是否转移到后台运行
	if ( daemon_var )
	{
		daemon(0,0);
	}
    
	if (socket_init(&listen_fd,listen_port) < 0)
	{
		printf("socket_init error:[%s]\n",strerror(errno));
		return -1;
	}

	printf("listen_fd:[%d]\n",listen_fd);

   	while(1)
	{
	//开始accept过程
		if ((client_fd = accept(listen_fd,NULL,NULL)) < 0)
		{
			printf("accept error:[%s]\n",strerror(errno));
			close(listen_fd);
			return -2;
		}
		printf("client_fd[%d]\n",client_fd);
    
		while(1)
		{
			rv = read(client_fd,buf,sizeof(buf));
			if(rv<0)
			{
				printf("error or disconnect[%s]\n",strerror(errno));
		    	close(client_fd);
				return 0;
			}
			if(rv == 0)
			{
				printf("client disconnect and waiting new clientfd connet\n");
        	    close(client_fd);
				break;
			}
			printf("client message:[%dbytes]%s\n",rv,buf);

			if (write(client_fd,"Receive success\n",sizeof("Receive success\n")) < 0)
			{
				printf("error:[%s]\n",strerror(errno));
				continue;
			}
		}

		continue;//若客户端断开，rv=0后break，重新到accept环节去监听socketfd
	}

	return 0;
}



//打印使用信息函数
int help_printf( char *program )
{
	if (NULL == program )
	{
		printf("help_printf arguments error[%s]\n",strerror(errno));
		return -1;
	}
	printf("%s usage:\n",program);
	printf("--help (-h) : get help menu\n");
	printf("--port (-p) : listen port \n");
    printf("--daemon(-d): [0]un_daemon use ,[1]daemon use\n");
	return 0;
}

//socket_init初始化
int socket_init(int *listen_fd,int listen_port)
{
	struct sockaddr_in 					servaddr_in;
	int 								opt = 1; 
    printf("start init socket...\n");
	if ((*listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("socket failture:%s\n",strerror(errno));
		return -1;
	}
	//printf("listen_fd[%d]\n",*listen_fd);
    setsockopt(*listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	memset(&servaddr_in,0,sizeof(servaddr_in));
	servaddr_in.sin_family       = AF_INET;
	servaddr_in.sin_port		 = htons(listen_port);
    servaddr_in.sin_addr.s_addr  = htonl(INADDR_ANY);
	if (bind(*listen_fd,(struct sockaddr *)&servaddr_in,sizeof(servaddr_in)) < 0 )
	{
		printf("bind error[%s]\n",strerror(errno));
		close(*listen_fd);
		return -2;
	}
	printf("bind success!\n");

    if (listen(*listen_fd,MAX_LISTEN) < 0)
	{
		printf("listen error[%s]\n");
        close(*listen_fd);
		return -3;
	}
	printf("listen_fd[%d]\n",*listen_fd);
    printf("init_socket finish...\n");
	
	return 0;
}


