#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "main.h"
#include "socket_client.h"


int client_init(int port,char *serv_ip)
{
	int         con_fd=-1;
	int         rv=-1;
	struct      sockaddr_in serv_addr;

	con_fd=socket(AF_INET,SOCK_STREAM,0);

	if(con_fd<0)
	{
		printf("Create socket failure: %s\n",strerror(errno));
		return -1;
	}
	printf("Create socket[%d] successfully!\n",con_fd);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
	inet_aton(serv_ip,&serv_addr.sin_addr);

	if(con_fd>=0)
	{
		rv=connect(con_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		if(rv<0)
		{
			printf("Connect to server[%s:%d] failure: %s\n",serv_ip,port,strerror(errno));										
			return -2;						
		}								
		printf("Connect to server[%s:%d] successfully!\n",serv_ip,port);
	}
	return con_fd;
}


int socketconnected(int sockfd)
{
	struct            tcp_info info;
	int               len=sizeof(info);

	if(sockfd<=0)
	{
		printf("Sockfd is error!\n");
		return 0;
	}
	getsockopt(sockfd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&len);
	if(info.tcpi_state == 1)           //TCP_ESTABLISHED
	{
		printf("Server connected!\n");
		return 1;
	}
	else
	{
		printf("Server disconnected!\n");
		return 0;
	}
}


