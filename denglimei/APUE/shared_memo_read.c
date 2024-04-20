/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  shared_memo_read.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "08/04/24 23:17:47"
 *                 
 ********************************************************************************/

#define  FTOK_PATH  "/dev/zero"
#define  FTOK_PROJID 0x22

typedef struct st_student 
{
	char   name[64];
	int    age;
} t_student;

int main(int argc, char **argv)
{
	key_t      key;
	int        shmid;
	int        i;
	t_student  *student;

	if( (key = ftok(FTOK_PATH, FTOK_PROJID)) < 0 )
	{
		printf("ftok() get IPC token failure: %s\n", strerror(errno));
		return -1;
	}

	shmid = shmget(key, sizeof(t_student), IPC_CREAT|0666);
	if(shmid < 0)
	{
		printf("shmget() create shared memory failure: %s\n", strerror(errno));
		return -2;
	}
	
	student = shmat(shmid, NULL, 0);
	if( (void *)-1 == student )
	{
		printf(
	}


	
}


