/*********************************************************************************
 *
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  shared_memory.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(05/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "05/03/24 18:35:14"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/ipc.h>       
#include <sys/shm.h>
#include <unistd.h>

#define FTOK_PATH  "/home/iot24/zhouxinzhi/Apue/IPC/.memw"
#define FTOK_VALUE 0x22


typedef struct st_student
{
	char 			name[64];
	int				age;
}t_student;

int main(int argc,char *argv[])
{
	key_t						key = -1;
	int							shmid = -1;
	int							i;
	t_student 					*student;

	if ( (key = ftok(FTOK_PATH,FTOK_VALUE)) < 0)
	{
		printf("ftok() error:%s\n",strerror(errno));
		return -1;
	}
	printf("%d\n",key);

	if ((shmid = shmget(key,sizeof(t_student),IPC_CREAT|0666)) < 0)
	{
		printf("shmid error:%s\n",strerror(errno));
		return -2;
	}
	printf("shmid:%d\n",shmid);

	student = shmat(shmid,NULL,0);

	if ( (void*)-1 == student )
	{
		printf("shmat error:%s\n",strerror(errno));
		return -2;
	}

	strncpy(student->name,"zhangsan",sizeof(student->name));
	student->age = 18;

	for(i=0;i<4;i++)
	{
		student->age ++;
		printf("Student %s age [%d]\n",student->name,student->age);
		sleep(1);
	}

	shmdt(student);
	shmctl(shmid,IPC_RMID,NULL);

	return 0;

}

