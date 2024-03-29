/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  shared_men_read.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 20:31:04"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>


#define FTOK_PATH		"/dev/zero"
#define FTOK_PROJID		0x22

typedef struct st_student
{
	char		name[64];
	int			age;

}t_student;


int main(int argc,char **argv)
{
	key_t		key;
	int			shmid;
	int			i;
	t_student	*student;

	if( (key = ftok(FTOK_PATH,FTOK_PROJID)) < 0 )
	{
		printf("ftok() get IPC token failure:%s\n",strerror(errno));
		return -1;
	}

	shmid = shmget(key,sizeof(t_student),IPC_CREAT|0666);
	if( shmid < 0 )
	{
		printf("shmget() create shared memory failure : %s\n",strerror(errno));
		return -2;
	}

	student = shmat(shmid,NULL,0);

	if( (void *)-1 == student )
	{
		printf("shamt() alloc shared memory failure : %s\n",strerror(errno));
		return -2;
	}

	for(i=0;i<4;i++)
	{
		printf("Student '%s' age [%d]\n",student->name,student->age);
		sleep(1);

	}

	shmctl(shmid,IPC_RMID,NULL);
	return 0;

}

