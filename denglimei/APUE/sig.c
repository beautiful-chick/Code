/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sig.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 22:51:28"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <signal.h>

int git = 0;

void sig_handle(int signum)
{
	printf("Catch SIGINT[%d]\n",signum);
	git = 1;
}
int main(int argc, char **argv)
{
	signal(SIGINT, sig_handle);
  	signal(SIGTERM, sig_handle);

	while( !git )
	{
		;
	}

	printf("you ya bu shi bai lan\n");
	return 0;
}



