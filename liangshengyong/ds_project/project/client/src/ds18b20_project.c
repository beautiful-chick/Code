/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20_project.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "08/03/24 22:39:43"
 *                 
 ********************************************************************************/

#include "ds18b20_project.h"
#define  BUFSIZE 100
float get_temp(void)
{
	int 		fd = -1;
	int 		rv = -1;
	char 		buf[BUFSIZE];
	float 		temp;
	char 		*ptr = NULL;
	memset(buf,0,sizeof(buf));
	fd = open("/sys/bus/w1/devices/28-0317320a8aff/w1_slave",O_RDONLY);

	if(fd < 0)
	{
		printf("open ds18b20 failure:%s\n",strerror(errno));
		return -1;
	}

	rv = read(fd,buf,sizeof(buf));

	//printf("%d\n",rv);
	ptr = strstr(buf,"t="); 
	if(rv < 0)
	{
		printf("read fd failure:%s\n",strerror(errno));
		return -2;
	}

	if(ptr == NULL)
	{
		printf("strstr failure:%s\n",strerror(errno));
		return -3;
	}
	ptr += 2;
	temp = atof(ptr);
	//printf("%.2f\n",temp/1000);
	close(fd);
	return temp;

}
