/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  string_handle.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 19:46:14"
 *                 
 ********************************************************************************/

#ifndef STRING_HANDLE
#define STRING_HANDLE

#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "temp.h"

extern int getdate_time(struct tm *get_tm,char* buf);
extern int report_string(t_temp_measure *str,char *buf);
extern int apart_string(char *str,char* cut_mark,void* buf);
#endif 
