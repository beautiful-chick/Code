/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "17/02/24 17:52:09"
 *      ds18b20温度传感器本身也就是一个文件，所以我们可以通过打开他的方法来实现采集温度
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int measure_temp(float *temp);

int main(int argc,char *argv[])
{
	float 				   temp = 0;

	measure_temp(&temp);

	printf("the temp:%f\n",temp);

	return 0;
}

int measure_temp(float *temp)
{
	int					   fd = -1;//打开ds18b20时候的文件描述符
	int					   found = -1;//是否找到chip的参数。
	char            	   *w1_path = "/sys/bus/w1/devices/"; //文件名的前半段,28-后面不能确定，因为设备号会变化，我们一会获取。
	char				   chip[32];//文件名的后半段，-28
	char				   ds_path[128];//拼凑出全部的路径名
    DIR					   *dirp = NULL;//打开w1时返回的DIR类型文件指针
    struct dirent          *direntp = NULL;//是readdir后返回的文件夹信息结构体指针
    char				   buf[1024];
    char 				   *ptr;//用来指向温度值

	if (NULL == temp)
	{
		printf("mesure_temp arguments error:[%s]\n",strerror(errno));
		return -1;
	}
	printf("measure_temp arguments right!\n");
    
	//打开devices文件夹
	if ( NULL == (dirp = opendir(w1_path)) )
	{
		printf("open w1_path error:[%s]\n",strerror(errno));
		return -2;
	}
	printf("open w1_path DIR[%p] success!\n",dirp);

	//readdir查找这个文件夹下有什么
	while(NULL != (direntp=readdir(dirp)))
	{
		if (strstr(direntp->d_name,"28-"))
		{
			strncpy(chip,direntp->d_name,sizeof(chip));
			found = 1;
		}
	}
	closedir(dirp);

	if (found < 0)
	{
		printf("nofind \"-28\" \n");
		return -3;
	}
	printf("find chip success[%s]\n",chip);
    
	snprintf(ds_path,sizeof(ds_path),"%s%s/w1_slave",w1_path,chip);
    printf("ds_path:%s\n",ds_path);

	if ((fd = open(ds_path,O_RDONLY)) < 0)
	{
		printf("open error[%s]\n",strerror(errno));
		return -4;
	}

	//read fd 
	memset(buf,0,sizeof(buf));
	if ( read(fd,buf,sizeof(buf)) <=0 )
	{
		printf("error:[%s]\n",strerror(errno));
		return -5;
	}
	//printf("temperature:[%s]\n",buf);

	if ( NULL == (ptr = strstr(buf,"t=") ))
	{
		printf("nofound error[%s]\n",strerror(errno));
		return -6;
	}

	//printf("%s\n",ptr);
	ptr += 2;

	*temp = atof(ptr)/1000;

	return 0;
}

