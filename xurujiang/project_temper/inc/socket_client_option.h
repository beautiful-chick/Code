/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client_option.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(21/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "21/03/24 18:54:28"
 *                 
 ********************************************************************************/
#ifndef SOCKET_CLIENT_OPTION_H
#define SOCKET_CLIENT_OPTION_H


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <libgen.h>
#include <pthread.h>
#include <ctype.h>
#include "ds18b20.h"//
#include <fcntl.h>
#include <sys/select.h>
#include "log.h"
#include <signal.h>
#include "get_time_temper.h"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8889
#define MSG_STR "Hello, Unix Network Program World!"
#define RETRY_INTERVAL 5  // 重连间隔（秒）


#define BUFFER_SIZE 1024 


void ds18b20_get_temperature(float *temp);
int sqlite3_temper(char* buf_temper);
int sqlite3_extract();

typedef struct socket_s
{
	int                     conn_fd ;
	char					*server_ip;
	int						port;
	int                     rv ;
}socket_t;

#endif
