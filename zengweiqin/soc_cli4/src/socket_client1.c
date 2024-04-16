/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client.c
 *    Description:  This file client get temperature.
 *                 
 *        Version:  1.0.0(06/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "06/04/24 17:06:24"
 *                 
 ********************************************************************************/

#include <getopt.h>	
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include "temp_setitimer.h"

static inline void print_usage(char *progname);
int socket_client_init(char *servip,int serv_port);

int main (int argc, char **argv)
{
	int					opt;
	int					serv_port=0;
	char				*servip = NULL;
	int					sam_time;
	char				*progname=NULL;
	char				buf[1024];
	int     			sockfd=-1;
	int     			rv=-1;
	float				temp;
	char				id[16];
	char				message[1024];
	char				time_str[32];
	struct  			sockaddr_in servaddr;
	struct				Data
	{
		float			temp1;
		char			time[32];
		char			num[16];
	}data;
	struct option       long_options[]=
 	{   
		{"ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"samptime",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};  
	
	progname=basename(argv[0]);//存放程序的名字
	
	while((opt = getopt_long(argc,argv,"i:p:t:h",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'i':
					servip=optarg;
					printf("ip is:%s\n",servip);//由命令行传进来的ip存放，后面程序里的ip都是这个
					break;
			case 'p':
					serv_port=atoi(optarg);
					printf("port:%d\n", serv_port);
					break;
			case 't':
					sam_time=atoi(optarg);
					printf("simple time is %d min.\n",sam_time);
					break;
			case 'h':
					print_usage(progname);
					break;
			default:
					break;

		}

	}

	if(!serv_port||!servip)
	{
		print_usage(progname);
	}

	sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
	{   
		printf("Create socket failure:%s\n",strerror(errno));
		return -1; 
	}   
	printf("Create socket[%d] successfully!\n",sockfd);
	//连接server
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(serv_port);
	inet_aton(servip,&servaddr.sin_addr);

	rv=connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(rv<0)
	{   
		printf("Connect to server[%s:%d] failure:%s\n",servip,serv_port,strerror(errno));
		return -2; 
	}   
	printf("Connect to server[%s:%d] successfully!\n",servip,serv_port);


 	signal(SIGALRM,handle_signal);//函数类型的指针，存放函数的名字
     set_time(sam_time, handle_signal);//定时


	while(1)
	{
		pause();
		get_temperature(&temp);
		get_id(id,sizeof(id));
		get_time(time_str,sizeof(time_str));
		
		data.temp1=temp;
		strcpy(data.num,id);
		//data.num=id;
		strcpy(data.time,time_str);
		//data.time=time_str;
		printf("序列号：%s,时间：%s,温度：%.2f\n",data.num,data.time,data.temp1);
		memset(message,0,sizeof(message));
		memcpy(message,&data,sizeof(data));
		rv=write(sockfd,message,sizeof(message));
		if(rv<0)
		{
			printf("Write to server by  sockfd[%d] failure:%s\n",sockfd,strerror(errno));
			return -3;
		}
		printf("Write to server by  sockfd[%d] successfully!\n",sockfd);

		memset(buf,0,sizeof(buf));
		rv=read(sockfd,buf,sizeof(buf));
		if(rv<0)
		{
			printf("Read data from server[%d] failure:%s\n",sockfd,strerror(errno));
			break;
		}
		else if(rv==0)
		{
			printf("Socket[%d] get disconnectefd\n",sockfd);
			break;
		}
		else if(rv>0)
		{
			printf("Read %d bytes from sockfd:%s",rv,buf);
		}
	}
	     close(sockfd);
	
} 
/*  
int socket_client_init(char *servip,int serv_port)
{
	struct sockaddr_in		servaddr;
	int						sockfd=-1;
	int						rv=-1;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
	{   
		printf("Create socket failure:%s\n",strerror(errno));
		return -1; 
	}   
	printf("Create socket[%d] successfully!\n",sockfd);
	//连接server
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(serv_port);
	inet_aton(servip,&servaddr.sin_addr);

	rv=connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(rv<0)
	{   
		printf("Connect to server[%s:%d] failure:%s\n",servip,serv_port,strerror(errno));
		return -2; 
	}   
	printf("Connect to server[%s:%d] successfully!\n",servip,serv_port);
}
*/

static inline void print_usage(char *progname)
{
	printf("Usage:%s [OPTION]...\n",progname);
	printf("%s is a socket server program,which used to verify client and echo bank string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");
	printf("-i[servip] Socket server ip address\n");
	printf("-p[port] Socket server port address\n");
	printf("-t[sample time] Socket server sample time\n");
	printf("-h[help] Display this help information\n");
	printf("\nExample:%s -i -p 192.168.198.129 8900\n",progname);
	return ;
}
