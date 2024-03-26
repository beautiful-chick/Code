/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(13/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "13/03/24 10:54:25"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include "ds18b20.h"

#define RPI_SIZE 16

int get_temperature(float *temp, char rpi[])
{
	int                    fd;
	char                   buf[128];
	char                  *ptr=NULL;
	int                    rv;
	DIR                   *dirp = NULL;
	char                   w1_path[64]="/sys/bus/w1/devices/";
	struct dirent         *direntp = NULL;
	int                    found = 0;
	char                  chip_sn[32];

	strncpy(rpi, "rpi001", RPI_SIZE);

	dirp = opendir(w1_path);
	if(!dirp)
	{
		printf("open foler %s failure: %s\n", w1_path, strerror(errno));
		return -1;
	}

	while((direntp = readdir(dirp)) != NULL )
	{
		if(strstr(direntp->d_name, "28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found = 1;
		}
	}
	closedir(dirp);

	if(!found)
	{
		printf("Can not find ds18b20 chipset\n");
		return -2;
	}

	strncat(w1_path, chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path, "/w1_slave", sizeof(w1_path)-strlen(w1_path));


	if((fd = open(w1_path, O_RDONLY)) < 0)
	{
		printf("open file failure: %s\n", strerror(errno))      ;
		return -3;
	}

	memset(buf, 0, sizeof(buf));
	if((rv = read(fd, buf, sizeof(buf))) < 0)
	{
		printf("Read data failure: %s\n", strerror(errno));
		return -4;
	}

	ptr = strstr(buf, "t=");

	if(ptr == NULL)
	{
		printf("Can not find t= string\n");
		return -5;
	}
	ptr+=2;

	*temp = atof(ptr)/1000;

	return 0;
}

