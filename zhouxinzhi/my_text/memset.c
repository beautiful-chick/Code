/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  memset.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(27/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "27/03/24 20:42:12"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[])
{
	int       a = 1;
	int 	  *p = NULL;

	p = &a;

	printf("p1:%p\n",p);

	memset(p,0,sizeof(*p));

	printf("p2:%p\n",p);

	return 0;

}




