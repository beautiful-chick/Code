/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  strtok.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "07/04/24 10:24:38"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
	char var[20] = "abc,def,ght";
	char *ptr = NULL;
	
	ptr = strtok(var,",");

	printf("1:%s\n",ptr);
	printf("2:%s\n",ptr);

	return 0;

}
