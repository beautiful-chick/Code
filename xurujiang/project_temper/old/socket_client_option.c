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
#include <sqlite3.h>
#include "log.h"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8889
#define MSG_STR "Hello, Unix Network Program World!"
#define RETRY_INTERVAL 5  // 重连间隔（秒）
void ds18b20_get_temperature(float *temp);  
int sqlite3_temper(char* buf_temper);
int sqlite3_extract();
#define BUFFER_SIZE 1024    
  
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
    int sockfd;  
    struct sockaddr_in server_addr;  
    char message[] = "Hello, server!";  
  
        int                     conn_fd ; 
        int                     rv = -1; 
        char                    buf[1024]; 
        struct 					sockaddr_in      serv_addr; 
		char					*server_ip = NULL;
		int 					ch;
		unsigned short			port=0;	
		char					buf_temper[1024];
		float					temp;

		time_t					time_now;
		char					buf_time[100];
		int						temp_time;
		fd_set 					rdset;
	    int                     maxfd; 
		int						ret;
		struct 					timeval timeout;
		int 					i,j;
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
								server_ip=optarg;
								break;
                        case 'p':
                                port=atoi(optarg);
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
    while (1) {  
	
    
	
        // 创建socket  
        conn_fd = socket(AF_INET, SOCK_STREAM, 0);  
        if (conn_fd < 0) {  
            perror("socket creation failed");  
            sleep(RETRY_INTERVAL);  
            continue;  
        }  
  		printf("时间间隔%d\n",temp_time);
        // 配置服务器地址信息  
        memset(&server_addr, 0, sizeof(server_addr));  
        server_addr.sin_family = AF_INET;  
        server_addr.sin_port = htons(port);  
        if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {  
            perror("invalid server address");  
            close(conn_fd);  
            exit(EXIT_FAILURE);  
        }  
		printf("ddddd\n");
  
        // 尝试连接到服务器  
        int qw = connect(conn_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if ( qw < 0) {  
            perror("connect failed\n");  
            close(conn_fd);  
            sleep(RETRY_INTERVAL);  
            
        }  
  
        printf("Connected to the server.\n"); 
		 
        if( qw >= 0)
        {
		    sqlite3 *db;  
    char *err_msg = 0;  
    int rc;  
      
    // 打开数据库连接  
    rc = sqlite3_open("data_temper.db", &db);  
      
    if (rc) {  
        fprintf(stderr, "无法打开数据库1: %s\n", sqlite3_errmsg(db));  
        return(0);  
    } else {  
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
			write(conn_fd, buf, strlen(buf)) ;
			printf("eqwieuioqw");
		}


	}
    // 准备SQL DELETE语句  
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
        // 连接成功后，开始发送消息  
        while (1) {  
        	   ds18b20_get_temperature(&temp);
             memset(buf_temper, 0, sizeof(buf_temper));
 
 
             memset(buf_time, 0, sizeof(buf_time));
 
             time(&time_now);
             char* buf_time = ctime(&time_now);
         sprintf(buf_temper,"temperature:%f,time:%s",temp,buf_time);
			if(write(conn_fd, buf_temper, strlen(buf_temper)) < 0)
        {
                printf("Write data to server [%s:%d] failure: %s\n", server_ip, port, strerror(errno));
				   int rvq= sqlite3_temper(buf_temper);            //写进数据库
			if(rvq == 0)
			{
				printf("写入数据库成功");
			}
			else
			{
				printf("写入数据库失败");
			}

      		    close(conn_fd);
				sleep(temp_time);
				break;
		}
        
		
	    
		
        		memset(buf, 0, sizeof(buf));
        		rv=read(conn_fd, buf, sizeof(buf));
        	if(rv < 0)
        {
         
			printf("Read data from server failure: %s\n", strerror(errno));
			close(conn_fd);
			break;
		}
				else if( 0 == rv )
                                                                             

        {
                printf("Client connect to server get disconnected\n");
        		close(conn_fd);
				break;
		}   
		     

        printf("Read %d bytes data from server: '%s'\n", rv, buf);  
            
            
            sleep(1); // 等待一秒后再发送下一条消息  
        }  
  
        // 关闭当前socket连接  
        close(conn_fd);  
        printf("Disconnected from the server. Retrying...\n");  
        sleep(temp_time);  
    }  
  
    return 0;  
}


