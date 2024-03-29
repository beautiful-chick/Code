/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  socket_pthread_server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "21/02/24 10:33:01"
 *                 多线程服务器如何实现，以及函数指针，指针函数的理解
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include<netinet/in.h>


typedef void* (THREAD_BODY) (void *thread_arg);//重新定义了一种叫THREAD_BODY的函数类型，即 void* 函数名（void *args）的类型
											  //即是pthread_create()函数中第三个参数的函数指针，所指向的那个指针函数。
											  
int start_pthread(pthread_t *tid,THREAD_BODY *pthread_workbody,void *thread_arg);//用来初始化并且创建一个新线程的函数
void* pthread_work1(void *pthread_arg);//用来处理线程的指针函数，监听client——fd和客户端通信
void help_printf(char *program);//打印帮助菜单

int main(int argc, char *argv[])
{
	int								rv = -1;
	int 							daemon_stop = 0;
	int 							listen_port = -1;
	int 							choo = -1;
	int 							client_fd = -1;
	int								listen_fd = -1;
	int 							on = 1;
	struct sockaddr_in				servaddr;
	pthread_t						tid = -1;
	struct option 					opts[]=
	{
		{"help",no_argument,NULL,'h'},
		{"port",required_argument,NULL,'p'},
		{"daemon",required_argument,NULL,'d'},
		{0,0,0,0}
	};

	while( (choo = getopt_long(argc,argv,"hp:d:",opts,NULL)) != -1 )
	{
		switch(choo)
		{
			case 'h':
				help_printf(argv[0]);
				return 0;
			case 'p':
				listen_port = atoi(optarg);
				break;
			case 'd':
				daemon_stop = atoi(optarg);
				break;
		}
	}

	if (listen_port < 0)
	{
		help_printf(argv[0]);
		return -1;
	}
	printf("will listen port:%d\n",listen_port);

    if (daemon_stop)
	{
		printf("will daemon use\n");
		daemon(0,0);
	}
	printf("no daemon use\n");

	if ( (listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
		printf("error:%s\n",strerror(errno));
		return -2;
	}

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port = htons(listen_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /*  listen all the IP address on this host */
	//inet_aton("192.168.0.16", &servaddr.sin_addr); /*  Only listen specify IP address on this host */

	rv=bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(rv < 0)
	{
		printf("Socket[%d] bind on port[%d] failure: %s\n", listen_fd,listen_port, strerror(errno));
		return -2;
	}
	listen(listen_fd, 13);

	while(1)
	{
		printf("start waiting connect...\n");
		if ( (client_fd = accept(listen_fd,NULL,NULL)) < 0)
		{
			printf("error;%s\n",strerror(errno));
			close(listen_fd);
	
		}

		start_pthread(&tid,pthread_work1,(void*)client_fd);//这里用void*而不是&client_fd是因为
		                                         	       //&client_fd相当于是（int*）类型了
													       //因为之前我们定义int listen_fd

	}

	close(listen_fd);

	return 0;
}

int start_pthread(pthread_t *tid,THREAD_BODY *pthread_workbody,void *thread_arg)
{
	pthread_attr_t					thread_attr;

	if ( pthread_attr_init(&thread_attr) )
	{
		printf("pthread_attr_init error:%s\n",strerror(errno));
		pthread_attr_destroy(&thread_attr);
		return -1;
	}
	printf("pthread_attr_init success:%p\n",&thread_attr);

    if ( pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED) ) 
	{
		printf("pthread_attr_setdetachstate error:%s\n",strerror(errno));
		pthread_attr_destroy(&thread_attr);
		return -2;
	}
	printf("pthread_attr_setdetachstate success !\n");

	if ( pthread_attr_setstacksize(&thread_attr,1024*120) )
	{
		printf("pthread_attr_setstacksize error:%s\n",strerror(errno));
		pthread_attr_destroy(&thread_attr);
		return -3;
	}
	printf("pthread_attr_setstacksize success !\n");
	
	if( pthread_create(tid,&thread_attr,pthread_workbody,thread_arg) ) 
	{
		printf("pthread_creat error:%s\n",strerror(errno));
		pthread_attr_destroy(&thread_attr);
		return -4;
	}
	printf("creat pthread success tid:%d\n",*tid);

	return 0;
}


void* pthread_work1(void *pthread_arg)
{
	int 								clie_fd = -1;
	int 								rv = -1;
	char 								buf[1024];
    
	if (pthread_arg == NULL)
	{
		printf("error:%s\n",strerror(errno));
	}
	clie_fd = (int)pthread_arg;
	printf("child tid:%d start connecting with client_fd:%d\n",pthread_self(),clie_fd);

	while(1)
	{
		memset(buf,0,sizeof(buf));
		rv = read(clie_fd,buf,sizeof(buf));
		if ( rv < 0 )
		{
			printf("error:%s\n",strerror(errno));
			close(clie_fd);
			pthread_exit(NULL);
		}
		if ( rv == 0 )
		{
			printf("client%d disconnect\n",clie_fd,strerror(errno));
			close(clie_fd);
			pthread_exit(NULL);
		}
		if ( rv > 0 )
		{
			printf("client%d message:%s\n",clie_fd,buf);

			if (write(clie_fd,"connect success\n",sizeof("connect success\n")) < 0)
			{
				printf("error:%s\n",strerror(errno));
				close(clie_fd);
				pthread_exit(NULL);
			}
		}
	}

	return 0;
}

//help打印菜单
void help_printf(char *program)
{
	printf("%s usage:\n",program);
	printf("--port(-p):listen_port\n");
	printf("--daemon(-d):daemon 1:daemon use 0:default no daemon use\n");

	return ;
}
