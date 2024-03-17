/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  time.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 19:14:58"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>


int main (int argc, char **argv)
{
	time_t             timep;
	struct tm         *p;
	char               buf[1024];

	memset(buf,0,sizeof(buf));

	time(&timep);
	p= localtime(&timep);
	//printf("current time:%d/%d/%d -- %d:%d:%d\n",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
    snprintf(buf,sizeof(buf),"current time:%d/%d/%d -- %d:%d:%d\n",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	printf("%s\n",buf);
	return 0;
} 

