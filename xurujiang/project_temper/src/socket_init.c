/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_init.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "21/03/24 18:41:16"
 *                 
 ********************************************************************************/
#include "socket_init.h"
int socket_temperature_init(socket_t *sock )
{

	struct 					sockaddr_in      server_addr; 
	int sock_rv=-1; 
	sock->conn_fd = socket(AF_INET, SOCK_STREAM, 0);  
	if (sock->conn_fd < 0)
	{  
		perror("socket creation failed");  
		sleep(1);  
		return -1;  
	}
	memset(&server_addr, 0, sizeof(server_addr));  
	server_addr.sin_family = AF_INET;  
	server_addr.sin_port = htons(sock->port); 

	if (inet_pton(AF_INET, sock->server_ip, &server_addr.sin_addr) <= 0) 
	{  
		perror("invalid server address");  
		close_sock(&sock) ;  
		return -2;
	}  

	sock_rv = connect(sock->conn_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if ( sock_rv < 0) {  
		perror("connect failed\n");  
		close_sock(&sock) ;
		sleep(1);  
		return sock_rv; 
	}  

	printf("Connected to the server.\n"); 
	return sock_rv;
}
