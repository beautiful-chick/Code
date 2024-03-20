/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_pack.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(19/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "19/03/24 11:01:50"
 *                 
 ********************************************************************************/

#include "sys/types.h"
#include "sys/socket.h"
#include "unistd.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include <netinet/in.h>
#include <arpa/inet.h>

int socket_init(char *server_ip,int port);

int socket_write(int socketfd,char *buf,int bytes);

int socket_read(int socketfd,char *buf,int bytes);
