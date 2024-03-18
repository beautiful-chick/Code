/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_devid.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(13/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "13/03/24 23:58:54"
 *                 
 ********************************************************************************/

#include <stdio.h>

int get_devid(char *id,int len)
{
  static int sn;
  snprintf(id,len,"RPI%04d",sn);
  sn++;
  return 0;

}
