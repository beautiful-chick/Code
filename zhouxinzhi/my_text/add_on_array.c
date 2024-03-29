/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  add_on_array.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(26/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "26/03/24 11:10:48"
 *                 
 ********************************************************************************/
#include <stdio.h>

int main(int argc,char *argv[])
{
	int 				buf[10];

	printf("buf address:%p\n",buf);
	printf("buf[0] address:%p\n",&buf[0]);
	printf("buf[1] address:%p\n",&buf[1]);

	printf("\n");
	printf("buf + 1:%p\n",buf+1);
	printf("&buf +1:%p\n",&buf+1);\
	printf("&buf[0] + 1:%p\n",&buf[0]+1);

	return 0;
}

