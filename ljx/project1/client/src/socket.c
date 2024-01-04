/*********************************************************************************
 *      Copyright:  (C) 2024 South-Central Minzu University
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2024年01月03日)
 *         Author:  lijinxuan <lijinxuan@gemial.com>
 *      ChangeLog:  1, Release initial version on "2024年01月03日 17时26分46秒"
 *                 
 ********************************************************************************/


#include<stdio.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include"socket.h"

int socket_init(info_t *sock_info,char *ip,int port)
{
        sock_info->fd = -1;
        if(ip == NULL)
        {
                memset(sock_info->hostip,0,sizeof(sock_info->hostip));
        }
        else
        {
                strncpy(sock_info->hostip,ip,sizeof(sock_info->hostip));
        }
        sock_info->port = port;
        sock_info->connectfd = 0;
        return 0;
}

int socket_client_connect(info_t *sock_info)
{
        struct sockaddr_in serveraddr;
        sock_info->fd = socket(AF_INET,SOCK_STREAM,0);
        if(sock_info->fd < 0)
        {
                printf("client socket create failure: %s\n",strerror(errno));
                return -1;
        }
        memset(&serveraddr,0,sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(sock_info->port);
        serveraddr.sin_addr.s_addr = inet_addr(sock_info->hostip);

        if(connect(sock_info->fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
        {
                printf("client connect to server failure: %s\n",strerror(errno));
                return -2;
        }
        sock_info->connectfd = 1;
        return 0;
}
int socket_diag(info_t *sock_info)
{
        struct tcp_info  info;
        int len;
        len = sizeof(info);
        if(sock_info->fd <= 0)
        {
                sock_info->connectfd = 0;
                return 0;
        }
        getsockopt(sock_info->fd,IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &len);
        if(info.tcpi_state == 1)
        {
                sock_info->connectfd = 1;
                return 1;
        }
        else
        {
                sock_info->connectfd = 0;
                return 0;
        }
}
