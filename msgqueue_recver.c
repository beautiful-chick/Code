/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  msgqueue_recver.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(15/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "15/03/24 09:03:54"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define FTOK_PATH		"/dev/zero"
#define FTOK_PROJID		0x22

typedef struct s_msgbuf
{
	long	mtype;
	char	mtext[512];
}t_msgbuf;


int main(int argc,char **argv)
{
	key_t		key;
	int			msgid;
	t_msgbuf	msgbuf;
	int			msgtype;
	int			i;

	if( (key = ftok(FTOK_PATH,FTOK_PROJID)) < 0 )
	{
		printf("ftok() get IPC token failure : %s\n",strerror(errno));
		return -1;
	}

	msgid = msgget(key,IPC_CREAT|0666);

	if( msgid < 0 )
	{
		printf("shmget() create shared memory failure : %s\n",strerror(errno));
		return -2;
	}

	msgtype = (int)key;
	printf("key[%d], msgid[%d] msgypte[%d]\n",(int)key,msgid,msgtype);

	for(i=0;i<4;i++)
	{
		memset(&msgbuf,0,sizeof(msgbuf));
		
		if( msgrcv(msgid,&msgbuf,sizeof(msgbuf.mtext),msgtype,IPC_NOWAIT) < 0 )
		{
			printf("msgsnd() receive message failure : %s\n",strerror(errno));
			break;
		}

		printf("Receive message : %s\n",msgbuf.mtext);
		sleep(1);
	}

	msgctl(msgid,IPC_RMID,NULL);
	return 0;
}

