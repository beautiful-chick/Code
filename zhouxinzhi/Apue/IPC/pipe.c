/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  pipe.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(23/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "23/02/24 16:27:29"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TEXT_STR "Hello child process!i am your parents process!\n"

int main(int argc,char *argv[])
{
	int  						pipe_fd[2];
	int 						rv = -1;
	pid_t						pid = -1;
	char						buf[512];//实现存储pipe两端通信的内容的数组
	int 						wstatus;

	if (pipe(pipe_fd) < 0)
	{
		printf("pipe error:%s\n",strerror(errno));
		return -1;
	}
	printf("create pipe success!\n");

	//创建一个子进程
	if ((pid = fork()) < 0)
	{
		printf("fork error:%s\n",strerror(errno));
		return -2;
	}
	if (pid == 0)//子进程
	{
		printf("child process start working\n");
		close(pipe_fd[1]);//关闭管道写端，子进程只进行读管道工作

		memset(buf,0,sizeof(buf));
		if ((rv = read(pipe_fd[0],buf,sizeof(buf))) < 0)
		{
			printf("read error:%s\n",strerror(errno));
			return -3;
		}
		printf("child process success recive %d bytes data from parents' process:%s\n",rv,buf);
		return 0;
	}
	if (pid > 0)
	{
		printf("parents' process starting working\n");
		close(pipe_fd[0]);//关闭管道读端，父进程只进行写管道工作

		if (write(pipe_fd[1],TEXT_STR,sizeof(TEXT_STR)) < 0)
		{
			printf("write error:%s\n",strerror(errno));
			return -4;
		}

		printf("parent process start waiting child process exit!\n");

		wait(&wstatus);

		return 0;
	}
	return 0;
}

