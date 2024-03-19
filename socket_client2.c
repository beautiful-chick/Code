/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lizhao>
 *                  All rights reserved.
 *
 *       Filename:  socket_client2.c
 *    Description:  This filei 
 *                 
 *        Version:  1.0.0(29/01/24)
 *         Author:  lingyun <lizhao>
 *      ChangeLog:  1, Release initial version on "29/01/24 22:02:23"
 *                 
 ********************************************************************************/
/* ***********************************
 * TCP客户端编程流程
 * 1.创建socket()
 * 2.阻塞，直到客户端进行连接
 * 3.建立连接connect()
 * 4.请求数据write()
 * 5.回应数据read()
 * 6.结束连接write()
 * *************************************/
#include <sys/types.h>      
#include <sys/socket.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<stdlib.h>
#include<getopt.h>

#define MSG_STR "Hello LingYun Iot Studio\n"
void print_usage(char *progname)
{
	printf("%s usage: \n",progname);
	printf("-i(--ipaddr): sepcify server IP address\n");
	printf("-p(--port): sepcify server port.\n");
	printf("-h(--help):print this help infoemation.\n");
	return ;
}
int main(int argc,char **argv)
{
	int                         sockfd = -1;
	int                         rv = -1;//connect文件描述符
	struct sockaddr_in          servaddr;
	char                        *servip = "192.168.0.8";
	int                         port = 12345;
	char                        buf[1024];

	struct option 				opts[] = {
			{"ipaddr",required_argument,NULL,'i'},
			{"port",required_argument,NULL,'p'},
			{"help",no_argument, NULL,'h'},
			{0,0,0,0}
	};

	int							ch;
	int							idx;

	while((ch = getopt_long(argc,argv,"i:p:h",opts,&idx)) != -1)
	{
		switch(ch)
		{
			case 'i':
						servip = optarg;
						break;
			case 'p':
						port = atoi(optarg);
						break;
			case 'h':
						print_usage(argv[0]);
						return 0;
		}

	}
	if(!servip || !port)
	{
		print_usage(argv[0]);
			return -1;
	}

	if(argc < 3)
	{
		printf("Program usage : %s[ServerIp] [Port]\n",argv[0]);
			return -1;
	}
	
	servip 	= argv[1];
	port 	= atoi(argv[2]);//将一个字符串转化成整型 

	/* 创建一个socket文件描述符，描述符类型为int型, int socket(int domain, int type, int protocol);*/
	//int sockfd = -1;
	sockfd = socket(AF_INET,SOCK_STREAM,0);

	if(sockfd < 0)
	 {
		printf("Create socket failure : %s\n",strerror(errno));
		 return -1;
	 }
		 printf("Create socket[%d] successfully\n",sockfd);
    /*现在connect（）钟已经指定目的IP地址和目的端口，利用bind（）也可以指定原端口和原ip地址,客户端不用bind,由操作系统内核自动分配*/

		// bind();
	/* 建立连接connect(),int connect(int sockfd, const struct sockaddr *addr,
			 socklen_t addrlen);*/

	/*int rv; = -1;//connect文件描述符
	struct sockaddr_in servaddr;
	char *servip = "192.168.0.8";
	int port = 12345;*/
	memset(&servaddr,0,sizeof(servaddr));
	/*填充connect()函数中的数*/
	servaddr.sin_family  = AF_INET;
	servaddr.sin_port    = htons(port);//转换成网络字节序
	inet_aton(servip,&servaddr.sin_addr);
	rv = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
	if(rv < 0)
	{
		printf("Connect to server[%s:%d] failure:%s\n",servip,port,strerror(errno));
		return -2;
	}
	printf("1\n");
	printf("Connect to server[%d:%s] successfully\n",servip,port);

	/*发送数据*/
	while(1)
	{
		rv = write(sockfd,MSG_STR,strlen(MSG_STR));
		if(rv < 0)
		{
			printf("Write to server by sockfd[%d],failure:%s\n",sockfd,strerror(errno));
			break;
		}
		printf("Write to server by sockfd[%d],successfully!\n",sockfd);
	
		/*读取数据*/
		//char buf = 1024;
		memset(buf,0,sizeof(buf));
		rv = read(sockfd,buf,sizeof(buf));
		if(rv < 0)
		{
			printf("Read data from server by sockfd[%d] failure : %s\n",sockfd,strerror(errno));
			break;
		}
		else if(rv == 0)
		{
			printf("Socket[%d] disconnected:%s\n",sockfd,strerror(errno));
		}
		else if(rv > 0)
		{
			printf("Read %d bytes data from seerver :%s\n",rv,buf);
		}


	}
	close(sockfd); 
}

										


