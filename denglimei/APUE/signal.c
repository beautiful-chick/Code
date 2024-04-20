/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  signal.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 17:41:17"
 *                 
 ********************************************************************************/
#include<signal.h>
#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>

int      g_sigstop = 0;

void signal_stop(int signum)
{
	if( SIGTERM == signum)
	{
		printf("SIGTERM signal detected\n");
	}
	else if( SIGALRM == signum)
	{
		printf("SIGALRM signal detected\n");
		g_sigstop = 1;
	}
}

void signal_user(int signum)
{
	if( SIGUSR1 == signum)
	{
		printf("SIGUSR1 signal detected\n");
	}
	else if( SIGUSR2 == signum )
	{
		printf("SIGUSR2 signal detected\n");
	}

	g_sigstop = 1;
}

void signal_code(int signum)
{
	if( SIGBUS == signum)
	{
		printf("SIGBUS signal detected\n");
	}
	else if( SIGILL == signum)
	{
		printf("SIGILL signal detected\n");
	}
	else if( SIGSEGV ==signum)
	{
		printf("SIGSEGV signal detected\n");
	}

	exit(-1);
}

int main(int argc, char **argv)
{
	char                *ptr = NULL;
	struct  sigaction   sigact,sigign;

	//use signal() install signal
	
    signal(SIGALRM, signal_stop);
	signal(SIGALRM, signal_stop);

	signal(SIGBUS, signal_code);
	signal(SIGILL, signal_code);
	signal(SIGSEGV, signal_code);

	//use sigaction() install signal
	
	//Initialize the catch signal structure
	
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigact.sa_handler = signal_user;
	
	//set up the ignore signal

	sigemptyset(&sigign.sa_mask);
	sigign.sa_flags = 0;
	sigign.sa_handler = SIG_IGN;

	sigaction(SIGINT, &sigign, 0);

	sigaction(SIGUSR1, &sigact, 0);
	sigaction(SIGUSR2, &sigact, 0);

	printf("Program start running for 20 seconds..\n");
	alarm(20);

	while( !g_sigstop )
	{
		;
	}

    printf("Program start stop running...\n");

	printf("Invalid pointer operator will raise SIGSEGV signal\n");
	*ptr = 'h';

	return 0;
}


