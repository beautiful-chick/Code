/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  rlimit.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/03/24 19:17:52"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/resource.h>

void print_limits(char *name,int resource)
{
	struct rlimit limit;
	if(getrlimit(resource,&limit) < 0)
	{
		printf("getrlimit for %s failure : %s\n",strerror(errno));
		return ;
	}
	printf("%-15s",name);
	if(limit.rlim_cur == RLIM_INFINITY)
	{
		printf("(infinite)   ");
	}
	else
	{
		printf("%15ld",limit.rlim_cur);
	}
	if(limit.rlim_max == FLIM_INFINITY)
	{

	}
}

