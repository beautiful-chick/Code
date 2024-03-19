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
 * 2.bind()
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
#include <sys/types.h>          /*  See NOTES */
#include <sys/socket.h>

#define MSG_STR "Hello LingYun Iot Studio\n"
void print_usage(char *progname)
{
	printf("%s usage: \n",progname);
	printf("-p(--port): sepcify server listen port.\n");
	printf("-h(--help):print this help infoemation.\n");
	return ;
}
int main(int argc,char **argv)
{
	int                         sockfd = -1;
	int							clifd = -1;
	int                         rv = -1;//connect文件描述符
	struct sockaddr_in          servaddr;
	struct sockaddr_in          cliaddr;
	int                         port = 0;
	char                        buf[1024];
	socklen_t					len;
	struct option 				opts[] = {
		
			{"port",required_argument,NULL,'p'},
			{"help",no_argument, NULL,'h'},
			{0,0,0,0}
	};

	int							ch;
	int							idx;

	while((ch = getopt_long(argc,argv,"p:h",opts,&idx)) != -1)
	{
		switch(ch)
		{
	
			case 'p':
						port = atoi(optarg);
						break;
			case 'h':
						print_usage(argv[0]);
						return 0;
		}

	}
	if(!port)
	{
		print_usage(argv[0]);
			return -1;
	}

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
	//inet_aton(servip,&servaddr.sin_addr);
	servaddr.sin_addr.s_addr =htonl( INADDR_ANY);//监听所有IP

	rv = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
	if(rv < 0)
	{
		printf("Socket [%d] bind on port[%d]  failure:%s\n",sockfd,port,strerror(errno));
		return -2;
	}

	listen(sockfd,13);

	
	printf("Start  to listen on port [%d] successfully\n",port);

	/*发送数据*/
	memset(&len,0,sizeof(len));
	while(1)
	{

		clifd = accept(sockfd,(struct sockaddr *)&cliaddr,&len);
		if(clifd < 0)
		{
			printf("Accept new client failure :%s\n",strerror(errno));
			continue;
		}
		printf("accept new client[%s:%d]ssuccessfully\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

		memset(buf,0,sizeof(buf));

		rv = read(clifd,buf,sizeof(buf));
		
		if(rv < 0)
		{
			printf("Read data from client by sockfd[%d] failure:%s\n",clifd,strerror(errno));

			close(sockfd);
			continue;
		}
		else if(rv == 0)
		{
			printf("socket[%d] get disconnected\n",clifd);
			close(clifd);
			continue;
		}
		else if(rv > 0)
		{
			printf("Rad %d bytes data from Server:%s\n",rv,buf);
		}

	 rv = write(clifd,buf,sizeof(buf));
     
	 if(rv < 0)
	 {        
			printf("Write to client by sockfd[%d] failure:%s\n",clifd,strerror(errno));                                                                    
			close(clifd);
			continue;
	 }                                                            
 	 


	}
	close(sockfd); 
}

										


