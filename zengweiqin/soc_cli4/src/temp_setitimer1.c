/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.c
 *    Description:  This file DS18B20 get temperature in time. 
 *                 
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 15:05:36"
 *                 
 ********************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <time.h>
#include<stdlib.h>
#include<dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include<getopt.h>

#define g_timeout  0

int  set_time(int sam_time,void (*ptr)(int ));
int get_id(char *id,int len);
int get_temperature(float *t);
int get_time(char *time_str,int size);
void handle_signal(int sig);

int main(int argc, char *argv[])
{
	float				temp;
	int					m;
	int					opt;
	void (*ptr)        (int );//函数指针
	int					sam_time;
	char				id[16];
	char				time_str[32];
	struct itimerval 	timer;
	struct option       long_options[]=
	{
		 {"sam_time",required_argument,NULL,'t'},
		 {NULL,0,NULL,0}
	};
	
	//int					signal;
	while((opt = getopt_long(argc,argv,"t:",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 't':
				sam_time=atoi(optarg);
			    printf("simple time is %d s.\n",sam_time);
				break;
			default:
				break;
		}
	}

	if(!sam_time)
	{
		printf("no time.\n");
		return -1;
	}
	

	signal(SIGALRM,handle_signal);//函数类型的指针，存放函数的名字

	set_time(sam_time, handle_signal);
	while(1)
	{
		pause();
		get_temperature(&temp);
		get_id(id,sizeof(id));
		get_time(time_str,sizeof(time_str));
		printf("温度：%.2f\n",temp);
		printf("序列号：%s\n",id);
		printf("时间：%s\n",time_str);
	}

	return 0;
}

int  set_time(int sam_time,void (*ptr)(int ))
{
    struct itimerval    timer;
	int					m;

	timer.it_interval.tv_sec=sam_time;
	timer.it_interval.tv_usec=0;
	//设置时间到后立即触发SIGALRM信号
	timer.it_value.tv_sec=1;
	timer.it_value.tv_usec=0;

	m=setitimer(ITIMER_REAL, &timer, NULL);
	if(m<0)
	{
		printf("setitimer fixed time failure:%s\n",strerror(errno));
		return -1;
	}
		
	printf("fixed time beginning...\n");
	return 3;
}
																		



void handle_signal(int sig)
{

	printf("捕捉到的信号编号为：%d\n",sig);
	g_timeout==1;
	return ;
}

int get_time(char *time_str,int size)
{
	time_t 	cur_time;
	struct 	tm *local_time;
	
	cur_time = time(NULL);
	local_time = localtime(&cur_time);
	strftime(time_str, size,"%Y-%m-%d %H:%M:%S", local_time);
	return 2;
}
int get_id(char *id,int len)
{
	int sn=1;
	snprintf(id, len, "DSB%04d", sn);
	return 1;
}

int get_temperature(float *t)
{
	int				fd; 
	char        	buf[1024];
	char        	*ptr;
	DIR				*dirp=NULL;
	struct dirent	*direntp=NULL;
	char			w1_path[64]="/sys/bus/w1/devices/";
	char			chip[32];
	int				found = 0;
	char 			w1[32]="/w1_slave";

	dirp=opendir(w1_path);
	if( !dirp )
	{
		printf("open folder %s failure:%s\n",w1_path,strerror(errno));
		return 1;
	}
	
	while((direntp=readdir(dirp))!=NULL)
	{
		if(strstr(direntp->d_name,"28-"))
		{
			strncpy(chip,direntp->d_name,sizeof(chip));
			found=1;
		}	
	}
	closedir(dirp);

	if(!found)
	{
		printf("Can not find ds18b20\n");
		return -2;
	}
		
	strncat(w1_path,chip,sizeof(w1_path));
	strncat(w1_path,w1,sizeof(w1_path));

	fd=open(w1_path,O_RDONLY);
	if( fd < 0 )
	{
		printf("open file failure:%s\n",strerror(errno));
		return -3;
	}


	memset(buf,0,sizeof(buf));	
	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("read data from fd=%d failure:%s\n",fd,strerror(errno));
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Can not find t= string\n");
		return -5;
	}

	ptr=ptr+2;

	*t=(atof(ptr))/1000;

	close(fd);
    return 0;
}

