/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server_epoll.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 16:52:22"
 *                 
 ********************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<ctype.h>
#include<getopt.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<libgen.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<stdlib.h>

#define BACKLOG		13
#define ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0])
#define MAX_EVENTS	512



void set_socket_rlimit(void);

static inline void printf_usage(char *progname);

int socket_server_init(char *listen_ip,int listen_port);
//检查socket是否出错
int is_socket_exceptional(int *sockfd);
