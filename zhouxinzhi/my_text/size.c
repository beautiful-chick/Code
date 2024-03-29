/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  size.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(26/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "26/03/24 10:56:30"
 *                 
 ********************************************************************************/

#include <stdio.h>

int main()
{
	int 			buf[10];

	printf("buf : %p\n",buf);
	printf("sizeof buf: %d \n",sizeof(buf));
	printf("sizeof buf[0]: %d \n",sizeof(buf[0]));

	return 0;
}
