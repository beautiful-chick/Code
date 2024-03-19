/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  signal_ch6.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(12/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "12/03/24 14:31:59"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>


int g_child_stop = 0;
int g_parent_run = 0;

void sig_child(int signum)
{
	if(SIGUSR1 == signum)
	{
		g_child_stop = 1;
	}
}

void sig_parent(int signum)
{
	if(SIGUSR2 == signum)
	{
		g_parent_run = 1;
	}
}

int main(int argc,char **argv)
{
	int			pid;
	int			wstatus;

	signal(SIGUSR1,sig_child);
	signal(SIGUSR2,sig_parent);

	if( (pid = fork()) < 0 )
	{
		printf("Create child process failure : %s\n",strerror(errno));
		return -2;
	}

	else if(pid == 0)
	{
		/*child process can do something first here,then tell parent process to start running*/

		printf("Child process start running and send parent a signal\n");

		kill(getppid(),SIGUSR2);

		while( !g_child_stop )
		{
			sleep(1);
		}

		printf("Child process receive signal from parent and exit now\n");
		return 0;
	}

	printf("Parent hangs up utill receive signal from child\n");

	while( !g_parent_run )
	{
		sleep(1);
	}

	/*Parent process can do something here,then tell child process to exit*/

	printf("Parent start running now and send child a signal to exit.\n");
	kill(pid,SIGUSR1);


	/*Parent wait child process exit*/
	wait(&wstatus);
	printf("Parent wait child process die and exit now\n");
	return 0;
}



