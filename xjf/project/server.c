#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MSG_STR "OK-OK-OK-OK!!!!"

void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-p(--port):spceify server listen port.\n");
	printf("-h(--help):print this help information.\n");
	return;
}

int main(int argc, char **argv)
{
	int                   ch;
	int                   on=1;
	int                   rv=-1;
	int                   port=0;
	int                   listen_fd=-1;
	int                   client_fd=-1;
	char                  buf[1024];
	char                 *servip=NULL;
	socklen_t             len;
	struct sockaddr_in    serv_addr;
	struct sockaddr_in    cli_addr;
	struct option         opts[]={
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while((ch=getopt_long(argc,argv,"p:h",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'p':
				port=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!port)
	{
		print_usage(argv[0]);
		return 0;
	}

	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd<0)
	{
		printf("Create socket failure: %s\n",strerror(errno));
		return -1;
	}
	printf("Socket create fd[%d]\n",listen_fd);

	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	rv=bind(listen_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if(rv<0)
	{
		printf("Create socket failure: %s\n");
		return -2;
	}

	listen(listen_fd,13);
	printf("Start accept new client incoming...\n");

	while(1)
	{
		printf("\nStart waiting and accept new client connect...\n");

		client_fd=accept(listen_fd,(struct sockaddr*)&cli_addr,&len);
		if(client_fd<0)
		{
			printf("Accept new socket failure: %s\n",strerror(errno));
			continue;
		}
		printf("Accept new client[%s:%d] successfully\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));

		memset(buf,0,sizeof(buf));
		rv=read(client_fd,buf,sizeof(buf));
		if(rv<0)
		{
			printf("Read data from client socket[%d] failure: %s\n",client_fd,strerror(errno));
			close(client_fd);
			continue;
		}
		else if(rv==0)
		{
			printf("client socket[%d] disconnected\n");
			close(client_fd);
			continue;
		}
		else if(rv>0)
		{
			printf("Read %d bytes data from server: %s\n",rv,buf);
		}

		rv=write(client_fd,MSG_STR,strlen(MSG_STR));
		if(rv<0)
		{
			printf("Write %d bytes date back to client[%d] failure: %s\n",rv,client_fd,strerror(errno));
			close(client_fd);
			continue;
		}
		//printf("Close client socket[%d]\n",client_fd);
		//close(client_fd);
	}
	close(listen_fd);
}
