/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_time.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(13/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "13/03/24 23:59:25"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <time.h>

void get_time(char *t)
{
  time_t current_time;

  time(&current_time);

  struct tm *local_time=localtime(&current_time);

  sprintf(t,"%04d-%02d-%02d %02d:%02d:%02d", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

}
