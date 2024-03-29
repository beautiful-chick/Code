/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  signal.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(22/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "22/02/24 16:24:38"
 *      //若子进程先执行，那么给父进程发送信号，并且用while阻塞自己
 *      父进程开始执行，在执行结束后，给子进程发送信号，然后用wait来等待子进程退出
 *		然后子进程开始运行，结束后，程序结束
 *
 *		//若父进程先运行，会先在while(1)中阻塞，sleep然后在子进程运行，进行如上步骤
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int g_child_stop = 0;
int g_parent_run = 0;

void sig_child(int signum)
{
	if (SIGUSR1 == signum)
	{
		g_child_stop = 1;
	}

	return ;
}

void sig_parent(int signum)
{
	if (SIGUSR2 == signum)
	{
		g_parent_run = 1;
	}
	return ;
}

int main(int argc,char *argv[])
{
	int 					pid = -1;
	int 					wstatus;

	signal(SIGUSR1,sig_child);
	signal(SIGUSR2,sig_parent);

	if ((pid = fork()) < 0)
	{
		printf("pid error:%s\n",strerror(errno));
		return -1;
	}
	if (pid == 0)//子进程开始执行
	{
		printf("child process start and send a signum to parent process!\n");

		kill(getppid(),SIGUSR2);

		while(!g_child_stop)
		{
			sleep(1);//若子进程先执行，给父进程发完信号后sleep等待父进程工作并返回信号后才开始工作。
		}
		printf("parent has return signal,and child process will exit now!\n");
		return 0;
	}
	if (pid > 0)//父进程开始执行
	{
		printf("parent will work untill recive signal from child process!\n");
		while(!g_parent_run)
		{
			sleep(1);
		}
		printf("parent process starting...\n");
		//process could do something here
		
		kill(pid,SIGUSR1);

		printf("waiting for child process exit\n");
		wait(&wstatus);
		return 0;
	}
	
	return 0;
}
