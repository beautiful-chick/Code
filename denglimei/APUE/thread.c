/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  thread.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 22:31:18"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

void *thread_worker1(void *args);
void *thread_worker2(void *args);

int main(int argc, char **argv)
{
	int              shared_var = 1000;    //共享资源
	pthread_t        tid;     //线程号
	pthread_attr_t   thread_attr;       //线程属性结构体

	if( pthread_attr_init(&thread_attr) )  //初始化线程属性
	{
		printf("Pthread_attr_init() failure: %s\n", strerror(errno));
		return -1;
	}

	if( pthread_attr_setstacksize(&thread_attr, 120*1024) )
	{
		printf("Pthread_attr_setstacksize() failure: %s\n", strerror(errno));
		return -2;
	}

	if( pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) )
	{
		printf("Pthread_attr_setdetachstate() failure: %s\n", strerror(errno));
		return -3;
	}

	pthread_create(&tid, &thread_attr, thread_worker1, &shared_var);
	printf("Thread_worker1 tid[%ld] created ok\n", tid);

	pthread_create(&tid, NULL, thread_worker2, &shared_var);
	printf("Thread_worker2 tid[%ld] created ok\n", tid);

	pthread_attr_destroy(&thread_attr);

	pthread_join(tid, NULL);   //主线程等待子线程结束才执行后面的程序

	while(1)
	{
		printf("Main/Control thread shared_var: %d\n", shared_var);
		sleep(10);
	}

}

void *thread_worker1(void *args)
{
	int        *ptr = (int *)args;

	if( !args )
	{
		printf("%s() get invaild arguments\n",__FUNCTION__);
		pthread_exit(NULL);
	}
	
	printf("Thread worker1 [%ld] start running...\n",pthread_self());

	while(1)
	{
		printf("+++:%s before shared_var++:%d\n",__FUNCTION__, *ptr);
		*ptr += 1;
		sleep(2);
		printf("+++:%s after sleep shared_var:%d",__FUNCTION__, *ptr);
	}

	printf("Thread worker1 exi...\n");

	return NULL;
}

void *thread_worker2(void *args)
{
	 int        *ptr = (int *)args;

	 if( !args )
	 {
		 printf("%s() get inalid arguments\n", __FUNCTION__);
		 pthread_exit(NULL);
	 }

	 while(1)
	 {
		 printf("---:%s before shared_var++:%d\n", __FUNCTION__, *ptr);
		 *ptr += 1;
		 sleep(2);
		 printf("---:%s after sleep shared_var:%d\n", __FUNCTION__, *ptr);
	 }

	 printf("Thread worker 2 exit...\n");

	 return NULL;
}




