#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>

#define MSG_STR                "Hello Socket Server!"

void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):spceify server IP address\n");
	printf("-p(--port):spceify server port.\n");
	printf("-h(--help):printf this help information.\n");
	return;
}
int main(int argc, char **argv)
{
	int                     conn_fd = -1;
	int                     rv = -1;
	char                    buf[1024];
	struct sockaddr_in      serv_addr;
	int                     port=0;
	char                   *servip=NULL;
	struct option           opts[] = {
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	int                     ch;
	int                     idx;

	while((ch = getopt_long(argc,argv,"i:p:h",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'i':
				servip = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!servip || !port)
	{
		print_usage(argv[0]);
		return 0;
	}

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(conn_fd < 0)
	{
		printf("create socket failure: %s\n", strerror(errno));
		return -1;
	}
	printf("Create socket[%d] successfully!\n",conn_fd);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_aton( servip, &serv_addr.sin_addr );

	rv = connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if( rv < 0)
	{
		printf("connect to server [%s:%d] failure: %s\n", servip, port, strerror(errno));
		return -2;
	}
	printf("connect to server[%s:%d] successfully!\n",servip,port);

	while(1)
	{
		rv=write(conn_fd,MSG_STR,strlen(MSG_STR));
		if( rv < 0 )
		{
			printf("Write data to server [%s:%d] failure: %s\n", servip, port, strerror(errno));
			return -3;
		}

		memset(buf, 0, sizeof(buf));
		rv = read(conn_fd, buf, sizeof(buf));
		if(rv < 0)
		{
			printf("Read data from server failure: %s\n", strerror(errno));
			return -4;
		}
		else if( 0 == rv )
		{
			printf("Client connect to server get disconnected\n");
			return -5;
		}
		else if(rv > 0)
		{
			printf("Read %d bytes data from server: '%s'\n", rv, buf);
			return -6;
		}
	}
	close(conn_fd);

}

