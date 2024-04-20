/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  thread_lock.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 23:41:31"
 *                 
 *****************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

void *thread_worker1(void *args);
void *thread_worker2(void *args);


typedef struct worker_ctx_s
{
	int              shared_var;
	pthread_mutex_t  lock;
} worker_ctx_t;//新的类型名

int main(int argc, char **argv)
{
	worker_ctx_t     worker_ctx;
	pthread_t        tid;
	pthread_attr_t   thread_attr;

	worker_ctx.shared_var = 1000;
	pthread_mutex_init(&worker_ctx.lock, NULL);

	if( pthread_attr_init(&thread_attr) )
	{
		printf("pthread_attr_init() failure: %s\n", strerror(errno));
		return -1;
	}

	if( pthread_attr_setstacksize(&thread_attr, 120*1024) )
	{
		printf("pthread_attr_setstacksize() failure: %s\n", strerror(errno));
		return -2;
	}

	if( pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) )
	{
		printf("pthread_attr_setdetachstate() failure: %s\n", strerror(errno));
		return -3;
	}

	pthread_create(&tid, &thread_attr, thread_worker1, &worker_ctx);
	printf("Thread worker1 tid[%ld] created ok\n",tid);

	pthread_create(&tid, &thread_attr, thread_worker2, &worker_ctx);
	printf("Thread worker2 tid[%ld] created ok\n",tid);
	
	while(1)
	{
		printf("Main /Control thread shared_var: %d\n",worker_ctx.shared_var);
		sleep(10);
	}

	pthread_mutex_destroy(&worker_ctx.lock);

}

void *thread_worker1(void * args)
{
	worker_ctx_t *ctx  = (worker_ctx_t *)args;

	if( !args )
	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}

	printf("Thread worker1 1[%ld] start running...\n",pthread_self());

	while(1)
	{
		pthread_mutex_lock(&ctx->lock);

		printf("+++: %s before shared_var++: %d\n", __FUNCTION__, ctx->shared_var);
		ctx->shared_var++;
		sleep(2);
		printf("+++: %s after sleep shared_var: %d\n", __FUNCTION__, ctx->shared_var);
		
		pthread_mutex_unlock(&ctx->lock);

		sleep(1);
	}

	printf("Thread worker1 exit..\n");

	return NULL;
}

void *thread_worker2(void *args)
{
	worker_ctx_t   *ctx = (worker_ctx_t *)args;
	
	if( !args )
	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}

	printf("Thread worker2 [%ld] start running...\n", pthread_self());

	while(1)
	{
		if( 0 != pthread_mutex_trylock(&ctx->lock) )
		{
			continue;
		}

		printf("---: %s before shared_var++: %d\n", __FUNCTION__, ctx->shared_var);
		ctx->shared_var++;
		sleep(2);
		printf("---: %s after sleep shared_var: %d\n", __FUNCTION__, ctx->shared_var);

		pthread_mutex_unlock(&ctx->lock);

		sleep(1);
	}

	printf("Thread worker2 exit..\n");

	return NULL;


}

