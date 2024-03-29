/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  temperature.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "08/03/24 10:46:32"
 *                 
 ********************************************************************************/
#include "temp.h"

/*
 *	函数名称: meas_temp
 *	返回值: 返回一个temp_measure类型的结构体指针，这个指针包含设备的名称（28——）
 *			采集到的温度，以及一个tm结构体指针，成功返回结构体指针，失败返回NULL
 *	参数：void
 *	说明：用于ds18b20采集温度
 *
*/
struct temp_measure* meas_temp(void)
{
	int 				w1_fd = -1;
	int					found = -1;
	DIR					*dirp = NULL;
	struct dirent		*direntp = NULL;
	char 				*w1_path = "/sys/bus/w1/devices/";   //ds18b20的路径名
	char 				name_path[128];					 	 //设备号名称
	char				all_path[265];					  	 //总路径
	char				buf[1024];	
	char				*ptr = NULL;							
	time_t				timer;								 //时间戳
	struct tm           *timp;								 
	t_temp_measure      *rv = NULL;							 //返回的结构体指针

	if ( (dirp=opendir(w1_path)) < 0 ) 
	{
		printf("opendir w1_path failure:%s\n",strerror(errno));
		return NULL;
	}

	//找到设备名-28.并且存入name——path中和结构体成员dev——name中
	while ( NULL != (direntp=readdir(dirp)) )
	{
		if ( strstr(direntp->d_name,"28-") )
		{
			strncpy(name_path,direntp->d_name,sizeof(name_path));
			strncpy(temp_s.dev_name,direntp->d_name,sizeof(temp_s.dev_name));
			found = 1;
		}
	}

	if ( found < 0 )
	{
		printf("found decive failure:%s\n",strerror(errno));
		return NULL;
	}

	snprintf(all_path,sizeof(all_path),"%s%s/w1_slave",w1_path,name_path);

	//获取采样时间戳并转化为时间，并赋值给对应的结构体成员
	if ( time(&timer) < 0 )
	{
		printf("get UTC time error\n");
		return NULL;
	}

	if ( (timp = localtime(&timer)) == NULL)
	{
		printf("get localtime error:%s\n",strerror(errno));
		return NULL;
	}
	temp_s.meas_time=timp;
	
	if ( (w1_fd = open(all_path,O_RDONLY)) < 0 )
	{
		printf("%d\n",w1_fd);
		printf("open %s error:%s\n",all_path,strerror(errno));
		return NULL;
	}

	memset(buf,0,sizeof(buf));
	if ( read(w1_fd,buf,sizeof(buf)) < 0 )
	{
		printf("open w1_fd error:%s\n",strerror(errno));
		close(w1_fd);
		return NULL;
	}

	//对buf进行处理并且赋值温度给对应的结构体成员
	ptr = strstr(buf,"t=");
	ptr += 2;
	temp_s.temp = atof(ptr)/1000;

	rv = &temp_s;
	return rv;
}
