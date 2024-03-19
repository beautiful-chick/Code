/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  fork.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(27/02/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "27/02/24 11:52:55"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
int main(int argc,char **argv)
{
	pid_t		pid;
	printf("Parent process PID[%d]start running...\n",getpid());

	pid = fork();
	if(pid < 0)
	{
		printf("fork() create child process failure : %s\n",strerror(errno));
		return -1;
	}
	else if(pid == 0)
	{
		printf("Child process PID[%d] start running,my parent PID is [%d]\n",getpid(),getppid());
		return 0;
	}
	else
	{
		printf("Parent process PID[%d] continue running.and chid process PID is [%d]\n",getpid(),pid);
		return 0;
	}
	return 0;

}

