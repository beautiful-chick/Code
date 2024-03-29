/*********************************************************************************
       Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 11:43:48"
 *                 
 ********************************************************************************/
#include "string_handle.h"
#include "temp.h"
#include "database.h"
#include "debug.h"
#include "socket.h"

#define DATABASE_NAME "client.db"
#define TABLE_NAME "client"

int help_printf(char *program);   //打印帮助菜单
void send_m(int *fd,t_temp_measure *buf);

int main(int argc,char *argv[])
{
	t_temp_measure                  *temp = NULL;
	sqlite3							*db = NULL;
	int 							conn_fd = -1;
	int 							net_state = -1;
	char							*server_ip = NULL;
	int								server_port = -1;
	int								interval_time = 30;  //采样间隔时间
	int								daemon_status = 0;   
	int								choo = -1;
	char							report_buf[128];
	struct option					opts[]=
	{
		{"daemon",required_argument,NULL,'d'},
		{"ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"time",required_argument,NULL,'t'},      //采样间隔时间
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	//选项选择
    while ( (choo = getopt_long(argc,argv,"d:i:p:t:h",opts,NULL)) != -1 )
	{
		switch (choo)
		{
			case 'd':
				daemon_status = atoi(optarg);
				break;

			case 'i':
				server_ip = optarg;
				break;

			case 'p':
				server_port = atoi(optarg);
				break;

			case 't':
				interval_time = atoi(optarg);
				break;

			case 'h':
				help_printf(argv[0]);
				return 0;
		}
	}

	if ( (server_port < 1024) || (server_ip == NULL) )
	{
		help_printf(argv[0]);
		return -1;
	}

	if (daemon_status == 1)
	{
		printf("process will daemon use\n");
		daemon(0,0);
	}

	//创建数据库以及表
	debug_printf("starting to creat db and table\n");
	if ( (db = sqlite3_open_database(DATABASE_NAME)) == NULL )
	{
		printf("creat db error:%s\n",strerror(errno));
		return -2;
	}
	else
	{
		debug_printf("create db success!\n");

		if ( sqlite3_create_table(db,TABLE_NAME) < 0 )
		{
			printf("create table error:%s\n",strerror(errno));
			return -2;
		}	
		debug_printf("create table success!\n");
	}

	//联网
	if ( (conn_fd = socket_connect(server_ip,server_port)) < 0 )
	{
		debug_printf("disconnect\n");
	}
	debug_printf("connect\n");

	//循环从这里开始
	while(1)
	{
		sleep(interval_time); //采样间隔时间
		
		//测温
		if ( (temp = meas_temp() ) == NULL )//这里的指针指向了temp.h中的全局变量temp_s
		{
			printf("meas_temp error:%s\n",strerror(errno));
			return -3;
		}
		getdate_time(temp->meas_time,temp->ptime);
        
		memset(report_buf,0,128);
		report_string(temp,report_buf);
		
		debug_printf("%s\n",report_buf);
        
		//是否联网
		if ( (net_state = socket_connect_status(conn_fd)) < 0 )
		{
			printf("socket check error:%s\n",strerror(errno));
			return -5;
		}
		debug_printf("net_state:%d\n",net_state);

		//若联网
		if ( net_state == 1 )
		{
			printf("Connect server success:\n	Upload data and database\n");
			debug_printf("connect success and we will do something here\n\n");
			
			if ( write(conn_fd,report_buf,strlen(report_buf)) < 0 )
			{
				printf("write error:%s\n",strerror(errno));
				continue;
			}

			while ( sqlite3_get_table_data(db,TABLE_NAME,temp) )
			{
				debug_printf("database have data,starting update data now\n");

				send_m(&conn_fd,temp);

				sqlite3_delect_single(db,TABLE_NAME);
			}

			debug_printf("now database is null\n");
			continue;
		}

		//若不联网
		if ( net_state == 0 )
		{
			close(conn_fd);
			printf("Disconnect network:\n	The data will be temporarily stored in the database\n");
			debug_printf("disconnect and we will do something here\n\n");
			sqlite3_insert_single(db,TABLE_NAME,temp);
			sqlite3_query_table(db,TABLE_NAME);
			
			//重新联网
			if ( (conn_fd = socket_connect(server_ip,server_port)) > 0 )
			{
				continue;
			}
		}
	}
	//循环在这里结束
	
	sqlite3_close_database(db);

	return 0;
}

//打印帮助菜单
int help_printf(char *program)
{
	printf("%s usage:\n",program);
	printf("--daemon (-d):\nset 1 use daemon process\nset 0 (default) use undaemon process\n");
	printf("--ip (-i):\nserver ip you want connect\n");
	printf("--port (-p):\nserver port youy want connect\n");
	printf("--time (-t):\ninterval time of measure temperature\ndefault time:30s\n");
	printf("--help (-h):\nprintf help menu\n");

	return 0;
}

//发送消息函数
void send_m(int *fd,t_temp_measure *buf)
{
	char					rebuf[128];

	report_string(buf,rebuf);

	if ( write(*fd,rebuf,strlen(rebuf)) < 0)
	{
		debug_printf("write error:%s\n",strerror(errno));
	}

}
