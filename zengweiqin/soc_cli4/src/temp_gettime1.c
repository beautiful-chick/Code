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
#include<stdlib.h>
#include<dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include<getopt.h>
#include <time.h>

int get_id(char *id,int len);
int get_temperature(float *t);

int main(int argc, char *argv[])
{
	float				temp;
	int					m;
	int					opt;
	int					sam_time;
	int					len=16;
	int					rv;
	char				id[16];
	struct timeval 		start_time;//开始采样时间
	struct timeval		current_time;//当前时间
	struct tm 			*tm;//localtime的格式
	char 				time_str[64];//保存采样开始时间
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

	get_id(id,len);
	printf("序列号：%s\n",id);

	for( ; ;)
	{
		gettimeofday(&start_time, NULL);
		tm= localtime(&start_time.tv_sec);
		snprintf(time_str, sizeof(time_str), "%04d-%02d-%02d %02d:%02d:%02d",
								 tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
								 tm->tm_hour, tm->tm_min, tm->tm_sec);//保存时间
		printf("time is:%s\n",time_str);

		gettimeofday(&current_time, NULL);//获取当前时间

		while ((current_time.tv_sec - start_time.tv_sec) < sam_time)//每3s采一次样
		{
			gettimeofday(&current_time, NULL);//再次对当前时间采样,只到满足为止
		}
		 get_temperature(&temp);
		 printf("温度：%.2f\n",temp);
		start_time = current_time;
	}
	return 0;

}
int get_time(char *time_str,float *t1)
{
	
}


int get_id(char *id,int len)
{
	int sn=1;
//	char	sn1[16];
//	id=sn1;
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

