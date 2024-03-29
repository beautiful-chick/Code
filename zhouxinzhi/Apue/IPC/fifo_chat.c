/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  fifo.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(23/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "23/02/24 16:56:41"
 *                  命名管道named——pipe，也称为fifo的实验               
 ********************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define FIFO_FIFE1 ".fifo_chat1"
#define FIFO_FIFE2 ".fifo_chat2"

int g_stop = 0;

void sig_pipe(int signum);
void help_printf(char *program);

int main(int argc,char*argv[])
{
	int 						mode = -1;
	int 						fdr_fifo = -1;
	int 						fdw_fifo = -1;
	int 						rv = -1;
	fd_set						rdset;
	char						buf[1024];

	if (argc != 2)
	{
		help_printf(argv[0]);
		return -1;
	}

	mode = atoi(argv[1]);//决定工作模式

	//检查是否存在两个管道文件，若不存在建立对应的管道文件
	if (access (FIFO_FIFE1,F_OK))//access 成功返回0，失败返回-1；
	{
		printf("FIFO_FIFE: %s not exist,and we will create it now!\n",FIFO_FIFE1);
		mkfifo(FIFO_FIFE1,0666);
	}
	if (access(FIFO_FIFE2,F_OK))
	{
		printf("FIFO_FIFE: %s not exist, and we will creade it now!\n");
		mkfifo(FIFO_FIFE2,0666);
	}

	signal(SIGPIPE,sig_pipe);//安装信号，如果管道破裂那么打印相关错误内容

	//0mode start
	if (mode == 0)
	{
		if ( (fdr_fifo = open(FIFO_FIFE1,O_RDONLY)) < 0)
		{
			printf("open %s error:%s\n",FIFO_FIFE1,strerror(errno));
			return -1;
		}
		
		if ( (fdw_fifo = open(FIFO_FIFE2,O_WRONLY)) < 0)
		{
			printf("open %s error:%s\n",FIFO_FIFE2,strerror(errno));
			return -2;
		}

	}
	else if (mode == 1)
	{
		if ((fdw_fifo = open(FIFO_FIFE1,O_WRONLY)) < 0)
		{
			printf("open %s error:%s\n",FIFO_FIFE1,strerror(errno));
			return -1; 
		}

		if ((fdr_fifo = open(FIFO_FIFE2,O_RDONLY)) < 0)
		{
			printf("open %s error:%s\n",FIFO_FIFE1,strerror(errno));
			return -2; 
		}
	}

	printf("start chating with anthor program now!,please input message:\n");


    while(!g_stop)
	{

		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO,&rdset);
		FD_SET(fdr_fifo,&rdset);

		rv = select(fdr_fifo+1,&rdset,NULL,NULL,NULL);

		if (rv <= 0)	
		{
			printf("rv < 0 error:%s\n",strerror(errno));
			continue;
		}
	
		if (FD_ISSET(fdr_fifo,&rdset))
		{
			memset(buf,0,sizeof(buf));
			if ((rv = read(fdr_fifo,buf,sizeof(buf))) < 0)
			{
				printf("error:%s\n",strerror(errno));
				break;
			}
		
			else if(0 == rv)
			{
				printf("another pipe has exit\n");
				break;
			}
			printf("--->%s",buf);
		}
		if (FD_ISSET(STDIN_FILENO,&rdset))
		{
			memset(buf,0,sizeof(buf));
			fgets(buf,sizeof(buf),stdin);
			write(fdw_fifo,buf,strlen(buf));
		}
	}
	return 0;
}


//发生管道破裂的时候的处理函数
void sig_pipe(int signum)
{
	if (SIGPIPE == signum)
	{
		printf("get pipe broke signal and let program exit!\n");
		g_stop = 1;
	}

	return ;
}

void help_printf(char *program)
{
	printf("%s usage\n",program);
	printf("this program need run twice:\n1st need run with [0]\n2th need run with [1]\n");

	return ;
}
