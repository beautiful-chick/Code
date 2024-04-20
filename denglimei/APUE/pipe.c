/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  pipe.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(29/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "29/03/24 00:04:31"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MSG_STR "This message is from parent: Hello, child process!"

int main(int argc, char **argv)
{
	int     pipe_fd[2];
	int     rv;
	int     pid;
	char    buf[512];
	int     wstatus;

	if( pipe(pipe_fd) < 0 )
	{
		printf("Create pipe failure: %s\n", strerror(errno));
		return -1;
	}

	if( (pid = fork()) < 0 )
	{
		printf("Create child process failure: %s\n", strerror(errno));
		return -2;
	}
	else if(pid == 0)
	{
		close(pipe_fd[1]);//close write endpoint

		memset(buf, 0, sizeof(buf));
		rv = read(pipe_fd[0], buf, sizeof(buf));
		if(rv < 0)
		{
			printf("Child process read from pipe failure: %s\n", strerror(errno));
			return -3;
		}

		printf("Child process read %d bytes data from pipe: \"%s\"\n", rv, buf);
		return 0;
	}

	close(pipe_fd[0]);
	if( write(pipe_fd[1], MSG_STR, strlen(MSG_STR)) < 0 )
	{
		printf("Parent process write data to pipe failure: %s\n", strerror(errno));
		return -3;
	}

	printf("Parent start wait child process exit...\n");
	wait(&wstatus);
}

