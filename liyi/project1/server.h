/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  server.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 21:45:39"
 *                 
 ********************************************************************************/
#include <unistd.h>
#include<stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#define MAX_EVENTS 512
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define BACKLOG 64


int socket_server_init(char *listen_ip,int listen_port);

void set_socket_rlimit(void);

void print_usage(char *progname);


int accept_client(int* epoll_fd,int* listen_fd);
