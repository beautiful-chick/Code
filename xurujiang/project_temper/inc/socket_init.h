/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_init.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(21/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "21/03/24 18:42:28"
 *                 
 ********************************************************************************/

#ifndef SOCKET_INIT_H
#define SOCKET_INIT_H

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
#include "socket_client_option.h"
int socket_temperature_init(socket_t *sock );





#endif
