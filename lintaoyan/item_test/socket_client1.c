/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client1.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "10/04/24 16:07:37"
 *                 
 ********************************************************************************/
#include <netdb.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define	MSG_STR "Hello Server!" 

void display_usage(char *progname)
{
	
	printf ("%s usage: \n", progname);
	printf ("-n(--hostname): Hostname of the server.\n");
	printf ("-i(--ip): IP address of the Server.\n");
	printf ("-p(--port): specify server listen port.\n");
	printf ("-h(--help): print this help information.\n");

	return ;
}


int main (int argc, char **argv)
{
	int					    ch;
	char                   *ipaddr = NULL;
	char                   *hostname = NULL;
	int                     port;
	int						sock_fd = 0;
	struct sockaddr_in      ser_addr;
	int					    rv = -1;
	char                    buf[1024];

	struct option			opts[] = {
		{"hostname", required_argument, NULL, 'n'},
		{"ip", required_argument, NULL, 'i'},
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};


	while( (ch = getopt_long(argc, argv, "n:i:p:h", opts, NULL)) != -1 )
	{
		switch(ch)
		{
			case 'n':
				hostname = optarg;
				break;
			case 'i':
				ipaddr = optarg;
				break;
			case 'p':
				port = atoi(optarg);
			    break;
			case 'h':
				display_usage(argv[0]);
				return 0;
			default:
				display_usage(argv[0]);
				return 1;
		}
	}


	if( (!hostname && !ipaddr) || !port )
	{
		display_usage(argv[0]);
		return 0;
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0)
	{
		printf("socket() create sock_fd failure: %s\n", strerror(errno));
		return -1;
	}
	printf("socket() create sock_fd[%d] successfully!\n", sock_fd);

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);

	if(hostname)
	{
		struct hostent *hostinfo = gethostbyname(hostname);
		if(!hostinfo)
		{
			printf("Cannot resolve hostname: %s\n", hostname);
			return -2;
		}
		memcpy(&ser_addr.sin_addr, hostinfo->h_addr, hostinfo->h_length);
	}
	else if(ipaddr)
	{
		if(inet_aton(ipaddr, &ser_addr.sin_addr) == 0)
		{
			printf("Invalid IP address: %s\n", ipaddr);
			return -3;
		}
	}

	rv = connect(sock_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(rv < 0)
	{
		printf("Connect to server [%s:%d] failure: %s\n", ipaddr, port, strerror(errno));
		return -4;
	}
	printf("Connect to server[%s:%d] successfully1\n", ipaddr, port);

	while(1)
	{
		rv = write(sock_fd, MSG_STR, sizeof(MSG_STR));
		if(rv < 0)
		{
			printf("Write to server by scokfd[%d] failure: %s\n", sock_fd, strerror(errno));
			return -3;
		}
		
		memset(buf, 0, sizeof(buf));
		rv = read(sock_fd, buf, sizeof(buf));
		if(rv < 0)
		{
			printf("Read data from server: scokfd[%d] failure: %s\n", sock_fd, strerror(errno));
			return -4;
		}
		else if(rv == 0)
		{
			printf("Socket[%d] get disconnected\n",sock_fd);
			return -5;
		}
		else if(rv > 0)
		{
			printf("Read %d bytes from Server: %s\n", rv, buf);
		}

		sleep(1);
	}

	close(sock_fd);
	return 0;
} 
