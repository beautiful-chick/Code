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
#include "sqlite3.h"
#include "sqlit.h"
#include "socket_init.h"
#include "get_time_temper.h"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8889
#define MSG_STR "Hello, Unix Network Program World!"
#define RETRY_INTERVAL 5  // 重连间隔（秒）


#define BUFFER_SIZE 1024    

close_sock(socket_t *sock)
{
	close(sock->conn_fd);

}


void print_usage( char *progname)
{
	printf("%s usage:  n",progname);
	printf("_i( -- ipaddr): specify server Ip address\n");
	printf("-p(--port): specify server PORT\n");
	printf("-h(--help): print this help information\n");
	printf("-d(--time): print this time information\n");
	return;
}
int main(int argc, char** argv) {  


	char message[] = "Hello, server!";  
	char                    buf[1024]; 
	int 		         	rv=-1;
	int 					ch;
	char					buf_temper[1024];
	float					temp;
 	socklen_t				optlen;
    int 					sndbuf_size; 
	int            			time_set=0;
	int						temp_time;
	fd_set 					rdset;
	struct 					timeval timeout;
	socket_t sock;
	sock.conn_fd=-1;
	sock.server_ip=NULL;
	sock.port=0;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	int flags;
	struct  option          opts[]  ={
		{"ipaddr", required_argument, NULL, 'i'},
		{"port",required_argument,NULL,'p'},
		{"time",required_argument,NULL,'d'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	printf("start paser argements\n");
	while( (ch=getopt_long(argc,argv,"i:p:d:h",opts,NULL)) != -1)
	{
		switch(ch)
		{
			case 'i':
				sock.server_ip=optarg;
				break;
			case 'p':
				sock.port=atoi(optarg);
				break;

			case 'd':
				temp_time =atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return EXIT_SUCCESS;
			default:
				break;

		}


	}


	log_init("temper.log", LOG_DEBUG); // 初始化日志系统，设置日志级别为DEBUG  

	log_message(LOG_INFO, "Program started."); // 记录信息级别的日志  
	log_message(LOG_DEBUG, "This is a debug message."); // 记录调试级别的日志
	//	set_timer(60*60*24);
	signal(SIGPIPE, SIG_IGN);
	while (1) {  


		int  sock_rv = socket_temperature_init( &sock);//socket初始化 


		if( sock_rv >= 0&&time_set==1)
		{
			sqlite3_open_delect(&sock);


		}

		time_set = 1;
		printf("111111111");
		// 连接成功后，开始发送消息  
		while (1) {  
			printf("2222222\n");

			get_time_temper(buf_temper, temp);

			printf("3333333\n");
			   optlen = sizeof(sndbuf_size);

			if(write(sock.conn_fd, buf_temper, strlen(buf_temper)) < 0 || getsockopt(sock.conn_fd, SOL_SOCKET, SO_SNDBUF, &sndbuf_size, &optlen) < 0 )
			{

				sqlite3_write(buf_temper,&sock);
				close_sock(&sock);
				sleep(temp_time);
				break;
			}

			memset(buf, 0, sizeof(buf));
			rv=read(sock.conn_fd, buf, sizeof(buf));
			if(rv < 0)
			{

				printf("Read data from server failure: %s\n", strerror(errno));
				close_sock(&sock) ;
				break;
			}
			else if( 0 == rv )


			{
				printf("Client connect to server get disconnected\n");
				close_sock(&sock) ;
				break;
			}   


			printf("Read %d bytes data from server: '%s'\n", rv, buf);  


			sleep(1); // 等待一秒后再发送下一条消息  
		}  

		// 关闭当前socket连接  
		close_sock(&sock) ;
		printf("Disconnected from the server. Retrying...\n");  
		sleep(temp_time);  
	}  

	return 0;  
}


