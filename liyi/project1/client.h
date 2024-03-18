/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 14:35:53"
 *                 
 ********************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
#include<netdb.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#define MAX_EVENTS 512
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define SERIAL "bs182021111"

typedef struct ReportTemp {
	float 			temperature;
	char			serial[32];
	char			time[32];
}ReportTemp;



//创建套接字
int m_connect(int *sockfd,char *servip,int port);
//打印用法
void print_usage(char *progname);
//获取温度
int get_temperature(ReportTemp *report_temp);

int find_file(char temp_path[64],char des[32],char file_name[32]);

//int find_temp(const char temp_path[64]);

int get_info(ReportTemp  *report_temp);
//获取主机序列号
int get_serial(ReportTemp  *report_temp);

//将字符串中的数字部分整合
void digit_to_str(char *str,char *serial);

//初始化服务器端信息

//获取当前时间
int  get_time(ReportTemp  *report_temp);

//读
int client_socket_write(int fd);
//写
int client_socket_read(int fd);

int is_socket_exceptional(int* sockfd);
