/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_temp.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "16/03/24 16:11:19"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include "ds18b20.h"
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<time.h>
#include<errno.h>


int get_temp(float *temp)
{
   int              fd=-1;
   char             *dev_path="/sys/bus/w1/devices/";
   char             buf[128];
   char             chip[64];
   char             path[64];
   char             *ptr;
   DIR              *dirp = NULL;
   struct dirent    *direntp;
   int              found=0;

   dirp = opendir(dev_path);
   if( !dirp)
   {
     printf("opendir %s error:%s\n",dev_path,strerror(errno));
	 return -4;
   }

   while( NULL !=(direntp = readdir(dirp)) )
  {      
	  if( strstr(direntp->d_name, "28-") )
       {
		   strncpy(chip, direntp->d_name, sizeof(chip));
		   found= 1;
		   break;
	   }
  }

   closedir(dirp);

   if( !found )
   {
	   printf("can not find ds18b20 in %s\n",dev_path);
	   return -5;
   }

   snprintf(path,sizeof(path),"%s/%s/w1_slave",dev_path,chip);
   //strncat(dev_path, chip, sizeof(dev_path)-strlen(dev_path));
   //strncat(dev_path, "/w1_slave", sizeof(dev_path)-strlen(dev_path));

   if((fd=open(path,O_RDONLY)) < 0)
   {
     printf("open %s error:%s\n",dev_path,strerror(errno));
	 return -1;
   }

   memset(buf,0,sizeof(buf));
   if(read(fd,buf,sizeof(buf)) < 0)
   {
    printf("read %s error:%s\n",path,strerror(errno));
	return -2;
   }

   ptr=strstr(buf,"t=");
   if(!ptr)
   {
     printf("can not get temp!");
     return -3;
   }

   ptr+=2;
   *temp=atof(ptr)/1000;

   close(fd);
	return 0;
} 


