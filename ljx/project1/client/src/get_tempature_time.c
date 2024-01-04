/*********************************************************************************
 *      Copyright:  (C) 2024 South-Central Minzu University
 *                  All rights reserved.
 *
 *       Filename:  get_tempature_time.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2024年01月03日)
 *         Author:  lijinxuan <lijinxuan@gemial.com>
 *      ChangeLog:  1, Release initial version on "2024年01月03日 17时27分24秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include<errno.h>
int get_tempature(float *tem)
{
    int           fd = -1;
    int           found = 0;
    int           rv;
    char          *ptr = NULL;
    char          buf[128];
    char          chip_path[64];
    char          w1_path[64] = "/sys/bus/w1/devices/" ;
    DIR           *dirp;
    struct dirent *dient;
    if(NULL == (dirp = opendir(w1_path)))
    {
        printf("open %s faliure : %s\n",w1_path,strerror(errno));
        return -1;
    }

    while(NULL != (dient = readdir(dirp)))
    {
        if(strstr(dient->d_name,"28-"))
        {
            strncpy(chip_path,dient->d_name,sizeof(chip_path));
            found = 1;
        }
    }

    if( !found )
    {
        printf("Cannot find dir..\n");
        return -2;
    }

    closedir(dirp);

    strncat(w1_path,chip_path,sizeof(w1_path) - strlen(w1_path));
    strncat(w1_path,"/w1_slave",sizeof(w1_path) - strlen(w1_path));

    if((fd = open(w1_path,O_RDONLY)) < 0)
    {
        printf("open faliure : %s\n",strerror(errno));
        return -3;
    }
    memset(buf,0,sizeof(buf));
    if((rv = read(fd,buf,sizeof(buf))) <= 0)
    {
        printf("read data failure : %s\n",strerror(errno));
        return -4;
    }

    ptr = strstr(buf,"t=");
    if(!ptr)
    {
        printf("cannot find t=\n");
        return -5;
    }

    ptr += 2;
    *tem = atof(ptr)/1000;
    return 0;
}
