/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(12/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "12/03/24 16:39:43"
 *                 
 ********************************************************************************/


#ifndef SOCKET
#define SOCKET

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>    
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <libgen.h>
#include <linux/tcp.h>
#include <netinet/ip.h>


#define MAX_LISTEN 28

extern int socket_connect(const char* server_ip,int server_port);
extern int socket_connect_status(int socket_fd);
extern int socket_listen_init(const char* listen_ip,int listen_port);

#endif

