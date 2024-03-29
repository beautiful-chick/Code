/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  message.c
 *    Description:  This file ]
 *                 
 *        Version:  1.0.0(06/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "06/03/24 21:01:03"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FTOK_PATH "/dev/zero"
#define FTOK_NUM 0X22

typedef struct s_msgbuf
{
	long mtype;
	char mtext[512];

}t_msgbuf;

int main(int argc,char *argv[])
{
	key_t				key = -1;
	int					msgid = -1;
	int					msg_type = -1;
	t_msgbuf			msgbuf;
	int					i = -1;


	if ((key = ftok(FTOK_PATH,FTOK_NUM)) < 0)
	{
		printf("ftok() error:%s\n",strerror(errno));
		return -1;
	}

	printf("key:%d\n",key);


	if (access(FTOK_PATH,F_OK) < 0)
	{
		printf("error:no path\n");
		return -2;
	}
	printf("path access\n");

	if ( (msgid = msgget(key,IPC_CREAT|0666)) < 0)
	{
		printf("msgid get error:%s\n",strerror(errno));
		return -2;
	}
	printf("msgid:%d\n",msgid);

	msg_type =(int)key;

	msgbuf.mtype = 1000;

	for (i=0; i<4; i++)
	{
		msgbuf.mtype = msg_type;
		strcpy(msgbuf.mtext,"ping");

		if (msgsnd(msgid,&msgbuf,sizeof(msgbuf.mtext),IPC_NOWAIT) < 0)
		{
			printf("msgsnd() error:%s\n",strerror(errno));
			break;
		}
		printf("send message:%s\n",strerror(errno));

		sleep(2);
	}

	msgctl(msgid,IPC_RMID,NULL);

	return 0;
}
