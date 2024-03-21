/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_server.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 15:46:35"
 *                 
 ********************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "errno.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "sys/resource.h"
int socket_server_init(char *listen_ip, int listen_port);
void set_socket_rlimit(void);
