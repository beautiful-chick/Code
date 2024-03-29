/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  semap.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(05/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "05/03/24 15:38:31"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define FTOK_PATH "/home/iot24/zhouxinzhi/Apue/IPC/.semapdir"
#define FTOK_PROJID 0x22

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

int semaphore_init(void);
int semaphore_del(int semid);
int semaphore_p(int semid);
int semaphore_v(int semid);

int main(int argc,char *argv[])
{
	int 				semid = -1;
	pid_t 				pid = -1;
	int					i;

	if ((semid = semaphore_init()) < 0)
	{
		printf("init semaphore error:%s\n",strerror(errno));
		return -1;
	}
	printf("init semid success!\n");

	if ( (pid = fork()) < 0)
	{
		printf("fork error:%s\n",strerror(errno));
		return -2;
	}
	else if (0 == pid)
	{
		printf("child process starting...\n");
		sleep(3);
		printf("child process finish something\n");
		semaphore_v(semid);//v操作，来确保可以父进程可以运行

		sleep(1);
		printf("child process exit now\n");

		exit(0);
	}
	else if (pid > 0)
	{
		printf("Parent process use p opt to wait child process\n");
		semaphore_p(semid);//若父进程先执行，p操作会使其信号量值为-1，进而产生自身阻塞
		printf("child process has destroy semaphore and exit\n");

		sleep(3);
		semaphore_del(semid);
		return 0;
	}

}

//初始化semap信号集函数
int semaphore_init(void)
{
	key_t				key = -1;
	int					semid = -1;
	union semun			sem_union;

	if ( (key = ftok(FTOK_PATH,FTOK_PROJID)) < 0 )
	{
		printf("ftok get IPC value failure:%s\n",strerror(errno));
		return -1;
	}
	printf("creat key:%d\n",key);

	if ( (semid = semget(key,1,IPC_CREAT|0644)) < 0 )
	{
		printf("get semid failure:%s\n",strerror(errno));
		return -2;
	}
	printf("creat semid:%d\n",semid);
									
	sem_union.val = 0;//设置信号量值为0，实现互斥操作
	if (semctl(semid,0,SETVAL,sem_union) < 0)//将这个结构体的量值赋值给对应的信号序列号的第n-1个信号
	{
		printf("semctl error:%s\n",strerror(errno));
		return -3;
	}

	return semid;//返回信号量序列号
}

//删除信号级中的信号集合
int semaphore_del(int semid)
{
	union semun 			sem_union;

	if ( semctl(semid,0,IPC_RMID,sem_union) < 0)
	{
		printf("semctl delete semaphore error:%s\n",strerror(errno));
		return -1;
	}

	return 0;
}

//p(-)操作
int semaphore_p(int semid)
{
	struct sembuf			_sembuf;

	_sembuf.sem_num = 0;
	_sembuf.sem_op = -1;
 	_sembuf.sem_flg = SEM_UNDO;

	if (semop(semid,&_sembuf,1) < 0)
	{
		printf("p error:%s\n",strerror(errno));
		return -1;
	}

	return 0;
}

//定义一个v操作函数
int semaphore_v(int semid)
{
	struct sembuf 			_sembuf;

	_sembuf.sem_num = 0;
	_sembuf.sem_op = +1;
	_sembuf.sem_flg = SEM_UNDO;

	if (semop(semid,&_sembuf,1) < 0)
	{
		printf("v error:%s\n",strerror(errno));
		return -1;
	}

	return 0;
}

