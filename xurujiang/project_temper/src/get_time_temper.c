/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_time_temper.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "21/03/24 19:24:27"
 *                 
 ********************************************************************************/
#include "socket_client_option.h"
#include "get_time_temper.h"
char get_time_temper(char* buf_temper, float temp)
{

	time_t					time_now;
	char*					buf_time;
	ds18b20_get_temperature(&temp);	 
	memset(buf_temper, 0, sizeof(buf_temper));

	time(&time_now);
	buf_time = ctime(&time_now);
	sprintf(buf_temper,"temperature:%f,time:%s",temp,buf_time);
}

