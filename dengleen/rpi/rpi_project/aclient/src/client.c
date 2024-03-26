/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(26/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "26/03/24 17:01:34"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "client.h"



char *domain_name_resolution(char *serv_ip)
{
	int                       get_back = -1;
	struct addrinfo           hints;
	struct addrinfo          *res;
	struct addrinfo          *read;
	struct sockaddr_in       *addr;
	char   *error="ANAlyze failure";

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;


	get_back = getaddrinfo(serv_ip, NULL, &hints, &res);

	if(get_back != 0)
	{
		return error;
	}

	printf("ANAlyze successfully\n");

	for(read = res; read != NULL; read = read->ai_next)
	{
		addr = (struct sockaddr_in *)read->ai_addr;
		serv_ip=inet_ntoa(addr->sin_addr);
		printf("%s\n", serv_ip);
	}
	freeaddrinfo(res);

	return serv_ip;
}
	

int connect_server(char *serv_ip, int serv_port, int *conn_fd)
{

	struct sockaddr_in      serv_addr;
	int                     on = 1;

	*conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(*conn_fd < 0)
	{
		dbg_print("create socket failure: %s\n", strerror(errno));
		close(*conn_fd);
		return -1;
	}

	setsockopt(*conn_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);
	
	if(inet_aton(serv_ip, &serv_addr.sin_addr) == 0)
	{
		serv_ip =  domain_name_resolution(serv_ip);
		inet_aton(serv_ip, &serv_addr.sin_addr); 
	}

	if( connect(*conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		dbg_print("connect to server [%s:%d] failure: %s\n", serv_ip, serv_port, strerror(errno));
		close(*conn_fd);
		return -2;
	}
	return 0;

}

