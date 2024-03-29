/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 12:43:39"
 *                 
 ********************************************************************************/
#include "socket.h"
#include "debug.h"
/*	
 *	函数名称：scoket_connect
 *	返回值：int ，返回值<0则表示失败,>0则成功，返回对应socket套接字的文件描述符。
 *	参数：server_ip:服务器的ip地址 server_port:服务器的端口
 *	说明：客户端链接对应的server_ip的server_prot 
*/
int socket_connect(const char* server_ip,int server_port)
{
	int							client_fd =	-1;
	struct sockaddr_in			servaddr_in;

	if ( (client_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
	    debug_printf("socket() creat client_fd failure:%s\n",strerror(errno));
		return -1;
	}

	memset(&servaddr_in,0,sizeof(servaddr_in));
	servaddr_in.sin_family = AF_INET;
	servaddr_in.sin_port   = htons(server_port); 
	inet_aton(server_ip,&servaddr_in.sin_addr);
	if (connect(client_fd,(struct sockaddr *)&servaddr_in,sizeof(servaddr_in)) < 0)
	{
		debug_printf("connect error:%s\n",strerror(errno));
		return -2;
	}

	return client_fd;

}


/*
 * 函数名称: socket_listen_init
 * 返回值: int 返回值<0则表示失败,>0则成功，返回服务端socket套接字listen后的文件描述符
 * 		   但不进行accept。
 * 参数: listen_ip的点分十进制字符串以及int类型的listen_port
 * 说明：用于初始化服务端的socket套接字，使其变为listen类型。 
*/
int socket_listen_init(const char* listen_ip,int listen_port)
{
	int									listen_fd = -1;
	int 								on = 1;
	struct sockaddr_in					servaddr_in;

	if ( (listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
		printf("create socket fd error:%s\n",strerror(errno));
		return -1;
	}

	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	servaddr_in.sin_family = AF_INET;
	servaddr_in.sin_port   = htons(listen_port);
	
	if ( listen_ip == NULL )
	{
		servaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		if ( inet_pton(AF_INET, listen_ip, &servaddr_in.sin_addr) <= 0 )
		{
			printf("inet_pton error:%s\n",strerror(errno));
			close(listen_fd);
			return -2;
		}
	}

	if ( bind(listen_fd,(struct sockaddr *)&servaddr_in,sizeof(servaddr_in)) < 0 )
	{
		printf("bind error:%s\n",strerror(errno));
		return -2;
	}

	if ( listen(listen_fd,MAX_LISTEN) < 0 )
	{
		printf("sock listen error:%s\n",strerror(errno));
		return -3;
	}

	return listen_fd;
}

/*
 *  函数名称:  socket_connect_status
 *  返回值: int 当返回值为0时，网络是断开的，当返回值为1时，网络是连接的。
 *  参数: int sock_fd
 *  说明: 用来判断socket的状态，判断其是否连接上了网络。
 */

int socket_connect_status(int socket_fd)
{
	struct tcp_info 					info;
	int 								len = -1;

	if (socket_fd <= 0)
	{
		return 0;
	}

	len = sizeof(info);

	getsockopt(socket_fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *) &len);

	if ( (info.tcpi_state == 1) )
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}
