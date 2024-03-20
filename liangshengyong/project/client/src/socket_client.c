/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "11/03/24 15:14:28"
 *                 
 ********************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "getopt.h"
#include "libgen.h"
#include "ds18b20_project.h"
#include "time.h"
#include "signal.h"
#include "sqlite3.h"
#include "socket_pack.h"

#define TABLE  "my_temperature"
static inline  void print_usage(char *progname);
void 		   sql_clear(sqlite3 *db);
void 		   sql_open_insert_iq(sqlite3 *db,char *buf,char *timeString,int connfd);

int 		flag  = 0; //标志位判断时间是否达到设定时间
int			flag2 = 1; //重连标志位
int			flag3 = 1; //保证flag2标志位的正常执行
int			flag4 = 0; //连接服务端成功后查询数据库有没有数据，如果有就优先发送
int			flag5 = 0; //防止连接服务端成功也插入数据到数据库
char		buff[100] = "../library/client_data.db";
//char		timeString[25];

int main(int argc,char **argv)
{
	int 				number;
	char				*server_ip;
	int					serv_port;
	int 				daemon_run = 0;
	int					opt;
	int 				connfd;
	char				buf[100];
	int 				rv = -1;
	char				*progname = NULL;
	int					num = 0;
	int 				in_minute;
	int 				time_minute;
	time_t 				current_time;
	struct 				tm *time_info;
	char 				timeString[20];
	time_t				currentTime;
	float				temp;
	sqlite3				*db;
	char				sql[256];
	FILE				*file = fopen("../library/client_log.txt","a+");
	sqlite3_stmt		*stmt;
	int 				rc;
	char				*err_msg=0;
	int 				data_exists = 0;
	current_time = time(NULL); //获取当前时间戳

	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info); //转换格式


	struct option long_options[]=
	{
		{"daemon",no_argument,NULL,'b'},
		{"server_ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"number",required_argument,NULL,'n'},
		{"minute",required_argument,NULL,'m'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	progname = basename(argv[0]);

	while((opt = getopt_long(argc,argv,"bi:p:n:m:h",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'b':
				daemon_run = 1;
				break;
			case 'i':
				server_ip = optarg; //
			case 'p':
				serv_port = atoi(optarg);
				break;
			case 'n':
				number=atoi(optarg);
				break;
			case 'm':
				in_minute=atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;
		}
	}
	time_minute = 60*in_minute;
	if(!serv_port)
	{
		print_usage(progname);
		return -1;
	}
	signal(SIGPIPE, SIG_IGN); //因为每次write失败后都会自动发送一个
	sprintf(buff,"client_data.db");


	sqlite3_open(buff,&db);

	sprintf(sql,"CREATE TABLE IF NOT EXISTS " TABLE "(TEMP TEXT NOT NULL);");
	sqlite3_exec(db,sql,0,0,0);
	memset(sql,0,sizeof(sql));
	while(1)
	{   
		currentTime = time(NULL);
		time_info = localtime(&currentTime);
		strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", time_info); //转换格式
		if((int)(currentTime - current_time) >= 5)
		{
			current_time = currentTime;
			flag = 1;
		}

		memset(buf,0,sizeof(buf));
		temp = get_temp();
		sprintf(buf,"number: %d - temperature: %.2f - time : %s ",number,temp/1000,timeString);

		/* */
		if(flag2 == 1)
		{
			if(flag == 1)
			{
				if((connfd = socket_init(server_ip,serv_port)) < 0)
				{
					fprintf(file,"this connect failure: %s\n -- time: %s\n",strerror(errno),timeString);
					fflush(file);
					close(connfd); 
					flag2 = 0;
					flag5 = 1;
				}
				else
				{
					flag2 = 0;
					flag3 = 0;
					flag4 = 1;
					fprintf(file,"Reconnect successfully -- time: %s\n",timeString);
					fflush(file);
					
				}
				//数据库的插入等操作
				sql_open_insert_iq(db,buf,timeString,connfd);

				flag = 0;
			}

		}


		if(flag == 1)
		{
			if((num = (socket_write(connfd,buf,sizeof(buf)))) <= 0)
			{
				close(connfd);
				fprintf(file,"write error:%s -- time: %s\n",strerror(errno),timeString);
				fflush(file);
				flag3 = 1; //断开后标志位置为1
			}		
			flag = 0;
		}

		if(flag3 == 1)
		{
			flag2 =1;
		}

	}
	close(connfd);

}

static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTIN].. \n",progname);

	printf(" %s is a socket server program, which used to verify client and echo back string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");
	printf(" -i[server_ip ] socket server ip address\n");
	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port ]Socket server port address\n");
	printf(" -h[help ] Display this help information\n");

	printf("\nExample: %s -b -p 8900\n", progname);
	return ;
}

void sql_clear(sqlite3 *db)
{
	char	 *err_msg = 0;
	int	 	 rc;
	char	 sql[100];
	sprintf(sql,"DROP TABLE IF EXISTS %s;",TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf( "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
	}
}

void sql_open_insert_iq(sqlite3 *db,char *buf,char *timeString,int connfd)
{
	char		 sql[256];
	char		 *err_msg = 0;
	sqlite3_stmt *stmt;
	int			 rc;
	FILE		 *file = fopen("client_log.txt","a+");
	if(sqlite3_open(buff,&db) != SQLITE_OK)
	{
		fprintf(file,"sqlite3_open failure: %s -- time: %s\n",sqlite3_errmsg(db),timeString);
		fflush(file);
		sqlite3_close(db);
	}
	if(flag5 == 1)
	{
		sprintf(sql, "insert into %s values ('%s');",TABLE,buf);
		rc = sqlite3_exec(db,sql,0,0,&err_msg);
		if(rc != SQLITE_OK)
		{
			fprintf(file,"sqlite3_exec failure: %s -- time: %s\n",sqlite3_errmsg(db),timeString);
			fflush(file);
			sqlite3_close(db);
		}
		flag5 = 0;
	}


	memset(sql,0,sizeof(sql));


	if(flag4 == 1) 
	{
		sprintf(sql,"select * from %s",TABLE);
		rc = sqlite3_prepare_v2(db,sql,-1,&stmt,0);
		if(rc != SQLITE_OK)
		{
			fprintf(file,"select failure: %s -- time: %S\n",sqlite3_errmsg(db),timeString);
			fflush(file);
			sqlite3_close(db);
		}
		memset(sql,0,sizeof(sql));
		int     j = sqlite3_column_count(stmt);

		while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			memset(buf,0,sizeof(buf));
			for(int i=0; i<j; i++)
			{
				const char *column = (const char *)sqlite3_column_text(stmt,i);
				sprintf(buf,"%s\n",column);
				write(connfd,buf,strlen(buf));

				sleep(1);
			}
		}
		j = 0;

		sql_clear(db); 
		sqlite3_close(db);
	}
}

