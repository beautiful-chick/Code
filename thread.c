/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  thread.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(03/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "03/03/24 14:03:59"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>


void	*thread_worker1(void *args);
void	*thread_worker2(void *args);

int main(int argc,char **argv)
{
	int					shared_var = 1000;
	pthread_t			tid;
	pthread_attr_t		thread_attr;

	/*pthread_addr_init()用默认值对现成属性对象进行初始化，&thread_attr指属性对象的地址*/

	if(pthread_attr_init(&thread_attr))
	{
		printf("pthread_addr_init() failure : %s\n",strerror(errno));
		return -1;
	}
	
	/*设置子线程中栈的大小*/

	if( pthread_attr_setstacksize(&thread_attr,120*1024))
	{
		printf("pthread_attr_setstacksize() failure : %s\n",strerror(errno));
		return -1;
	}

	/*设置线程是非分离状态还是分离状态，下面设置的是分离状态*/

	if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED))
	{
		printf("pthread_addr_setdetachstate() failure : %s\n",strerror(errno));
		return -1;
	}

	/*创建子线程worker1和worker2*/

	pthread_create(&tid,&thread_attr,thread_worker1,&shared_var);
	printf("Thread worker1 tid[%ld] created ok\n",tid);

	pthread_create(&tid,NULL,thread_worker2,&shared_var);
	printf("Thread worker2 tid[%ld] created ok\n",tid);

	/*pthread_attr_destory() 函数的意思是销毁线程的属性结构体，使它未初始化不能再次使用*/

	pthread_attr_destroy(&thread_attr);

	/*Wait until thread worker2 exit()*/
	pthread_join(tid,NULL);

	while(1)
	{
		printf("Main/Control thread shared_var : %d\n",shared_var);
		sleep(10);
	}
}

void *thread_worker1(void *args)
{
	int		*ptr = (int *)args;

	if( !args )
	{
		printf("%s() get invalid arguments\n",__FUNCTION__);
		pthread_exit(NULL);
	}
	printf("Thread worker1[%ld] start running...\n",pthread_self());

	while(1)
	{
		printf("+++ : %s before shared_var++ : %d\n",__FUNCTION__, *ptr);
		*ptr += 1;
		sleep(2);
		printf("+++ : %s after sleep shared_var : %d\n",__FUNCTION__, *ptr);

	}

	printf("Thread worker1 exit...\n");
	return NULL;
}


void *thread_worker2(void *args)
{
	int		*ptr = (int *)args;

	if( !args )
	{
		printf("%s() get invalid arguments\n",__FUNCTION__);
		pthread_exit(NULL);
	}
	printf("Thread worker2[%ld] start running...\n",pthread_self());

	while(1)
	{
		printf("--- : %s before shared_var++ : %d\n",__FUNCTION__, *ptr);
		*ptr += 1;
		sleep(2);
		printf("--- : %s after sleep shared_var : %d\n",__FUNCTION__, *ptr);
	}

	printf("Thread worker2 exit...\n");
	return NULL;
}



