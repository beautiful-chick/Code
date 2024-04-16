/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  temp_setitimer.h
 *    Description:  This file get temprature in time.
 *
 *        Version:  1.0.0(14/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "14/04/24 16:47:26"
 *                 
 ********************************************************************************/
#ifndef TEMP_SETITIME_H
#define TEMP_SETITIME_H 

#define g_timeout  0

int set_time(int sam_time,void (*ptr)(int ));
int get_id(char *id,int len);
int get_temperature(float *t);
int get_time(char *time_str,int size);
void handle_signal(int sig);

 
#endif

