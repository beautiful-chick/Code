/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client_temp.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "11/04/24 22:26:56"
 *                 
 ********************************************************************************/

void display_usage(char *progname);
void timer_handler(int signum);

int							 sock_fd = -1;

int main (int argc, char **argv)
{
	struct sockaddr_in		 ser_addr;
	struct hostent          *hostinfo = NULL:

	char					*hostname = NULL;
	char 					*ipaddr = NULL；
	int						 port= 0；
	int 					 interval = 1;
	int						 ch;
	int						 rv;

	struct option opts[] = {
		{"hosthome", required_argument, NULL, 'n'},
		{"ip", required_argument, NULL, 'i'},
		{"port", required_argument, NULL, 'p'},
		{"interval", required_argument, NULL, 't'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	while( (opt = getopt_long(argc, argv, "n:i:p:t:h")) != -1 )
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
			case 't':
				interval = atoi(optarg);
				break;
			case 'h':
				display_usage(argv[0]);
				exit(EXIT_SUCCESS);
			default:
				display_usage(argv[0]);
				exit(EXIT_SUCCESS);
		}
	}

	if( (!hostname && !ipaddr) || !port )
	{
		display_usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0)
	{
		printf("socket() create sock_fd failure: %s\n", strerror(errno));
		exit(EXIT_SUCCESS);
	}
	printf("socket() create sock_fd[%d] successfully!\n", sock_fd);

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);

	if(hosthome)
	{
		hostinfo = gethostbyname(hostname);
		if(!hostinfo)
		{
			printf("Cannot resolve hostname: %s\n", hostname);
			exit(EXIT_SUCCESS);
		}
		memcpy(&ser_addr.sin_addr, hostinfo->h_addr, hostinfo->h_length);
	}
	else if(ipaddr)
	{
		if(inet_aton(ipaddr, &ser_addr.sin_addr) == 0)
		{
			printf("Invalid IP address: %s\n", ipaddr);
			exit(EXIT_SUCCESS);
		}
	}

	rv = connect(sock_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(rv < 0)
	{
		printf("Connect to server failure: %s\n", strerror(errno));
		exit(EXIT_SUCCESS);
	}
	printf ("Connect to server successfully!\n");






	return 0;
} 

void display_usage(char *progname)
{
	printf("%s usage: \n", progname);
	printf("-n(--hostname): Hostname of the server.\n");
	printf("-i(--ip): IP address of the server.\n");
	printf("-p(--port): specify server listen port.\n");
	printf("-t(--interval): Sampling interval in seconds (default: 1 second).\n");
	printf("-h(--help): print this help information.\n");
}

void timer_handler(int signum)
{
	float   temp;
	int     rv;
	char    msg[64];
	rv = get_temperature(&temp);
	if (rv < 0)
	{
		printf("get temperature failure, return value: %d\n", rv);
		exit(EXIT_FAILURE);
	}


	snprintf(msg, sizeof(msg), MSG_STR, temp);

	rv = write(sockfd, msg, strlen(msg));
	if (rv < 0)
	{
		printf("Write to server failure: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Temperature sent to server: %.4f\n", temp);
}

}
}
}



