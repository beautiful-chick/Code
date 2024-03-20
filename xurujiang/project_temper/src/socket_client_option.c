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
#include <sqlite3.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8889
#define MSG_STR "Hello, Unix Network Program World!"
#define RETRY_INTERVAL 5  // 重连间隔（秒）


#define BUFFER_SIZE 1024    
void ds18b20_get_temperature(float *temp);  
int sqlite3_temper(char* buf_temper);
int sqlite3_extract();

typedef struct socket_s
{
	int                     conn_fd ;
	char					*server_ip;
	int						port;
	int                     rv ; 
}socket_t;
close_sock(socket_t *sock)
{
	close(sock->conn_fd);

}
void	sqlite3_write(char* buf_temper, socket_t *sock)
{
	printf("Write data to server [%s:%d] failure: %s\n", sock->server_ip, sock->port, strerror(errno));
	int rvq= sqlite3_temper(buf_temper);            //写进数据库
	if(rvq == 0)
	{
		printf("写入数据库成功");
	}
	else
	{
		printf("写入数据库失败");
	}
}
void	sqlite3_open_delect(socket_t *sock)
{
	int rc; 
	sqlite3 *db;  
	char *err_msg = 0; 
	char buf[200];
	rc = sqlite3_open("data_temper.db", &db);  

	if (rc) {  
		fprintf(stderr, "无法打开数据库1: %s\n", sqlite3_errmsg(db));  
		return 0;  
	} 
	else {  
		fprintf(stdout, "成功打开数据库1\n");  
	}
	const char *sql = "select * from company";
	sqlite3_stmt *stmt;
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);


	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL错误22:%s\n", sqlite3_errmsg(db));

	}
	else 
	{
		fprintf(stdout, "查询准备成功\n");
	}

	while( (sqlite3_step(stmt) == SQLITE_ROW))
	{
		int column_count = sqlite3_column_count(stmt);
		for(int i = 0; i < column_count; i++ )
		{
			const char *column_name = sqlite3_column_name(stmt, i);
			memset(buf, 0, sizeof(buf));
			sprintf(buf, column_name, sizeof(column_name));
			write(sock->conn_fd, buf, sizeof(buf)) ;

		}

		const char *sql2 = "DELETE  from company";  

		// 执行SQL语句  
		rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);  

		if (rc != SQLITE_OK ) {  
			fprintf(stderr, "SQL错误: %s\n", err_msg);  
			sqlite3_free(err_msg);  
		} else {  
			fprintf(stdout, "成功删除记录\n");  
		} 





	}
	// 准备SQL DELETE语句  




}

char get_time_temper(char* buf_temper, float temp)
{
	time_t					time_now;	
	char*					buf_time;	
	ds18b20_get_temperature(&temp);	 
	memset(buf_temper, 0, sizeof(buf_temper));

	time(&time_now);
	buf_time = ctime(&time_now);
	sprintf(buf_temper,"temperature:%f,time:%s",temp,buf_time);
}

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



	// 配置服务器地址信息  


	// 尝试连接到服务器  
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


