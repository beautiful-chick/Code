/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  temp.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 11:34:45"
 *                 
 ********************************************************************************/
#ifndef TEMP_H
#define TEMP_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct temp_measure
{    
	char        dev_name[128];
	float       temp;
	struct tm   *meas_time;
	char 		ptime[128];
}t_temp_measure;

static t_temp_measure temp_s;

extern struct temp_measure* meas_temp(void);

#endif

