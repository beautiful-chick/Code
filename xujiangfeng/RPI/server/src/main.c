#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <getopt.h>
#include <ctype.h>
#include <sqlite3.h>

#include "main.h"
#include "database.h"
#include "socket_server.h"
#include "sqlite3.h"

#define TABLE_NAME "RPI_num2"

int     g_sigstop=0;

int main(int argc, char **argv)
{
	int            daemon_run=0;
	int            clifd;
	int            listenfd;
	int            ch;
	int            rv=-1;
	int            port;
	int            j=0;
	char           *p;
	char           *pos;
	char           *buftok[3];
	pid_t          pid;
	struct         sigaction sigact,sigign;
	struct         option opts[] = {
		{"daemon",no_argument,NULL,'d'},
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};


	while( (ch=getopt_long(argc, argv, "dp:h", opts, NULL)) != -1 )
	{
		switch(ch)
		{
			case 'd':
				daemon_run=1;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}


	if( !port )
	{
		print_usage(argv[0]);
		return -1;
	}

	if(daemon_run)
	{
		if(daemon(1,1)<0)
		{
			printf("Running daemon failure:%s\n",strerror(errno));
			return -2;
		}
		printf("Running daemon successfully!\n");
	}

	sigemptyset(&sigign.sa_mask);
	sigign.sa_flags=0;
	sigign.sa_handler=SIG_IGN;

	sigemptyset(&sigact.sa_mask);
	sigign.sa_flags=0;
	sigign.sa_handler=signal_stop;

	sigaction(SIGUSR1,&sigact,0);
	sigaction(SIGUSR2,&sigact,0);
	sigaction(SIGPIPE,&sigign,0);

	sqlite_init();
	if(sqlite_table_exist(TABLE_NAME) == 0)
	{
		sqlite_create_table(TABLE_NAME);
	}

	set_socket_rimit();

	listenfd=socket_server_init(NULL,port);
	if(listenfd<0)
	{
		printf("Server listen on port %d failure!\n",port);
		return -3;
	}
	printf("Server start to listen on port %d\n",port);

	while(!g_sigstop)
	{
		printf("Start accept new client incoming...\n");
		clifd=accept(listenfd, NULL, NULL);
		if(clifd < 0)
		{
			printf("Accept new client failure: %s\n", strerror(errno));
			continue;
		}
		printf("Accept new client successfully\n");
		
		pid = fork();
		if( pid < 0 )
		{
			printf("fork() create child process failure: %s\n", strerror(errno));
			close(clifd);
			continue;
		}

		else if( pid > 0 )
		{
			/* Parent process close client fd and goes to accept new socket client again */
			close(clifd);
			continue;
		}
		else if ( 0 == pid )
		{
			char buf[1024];
			printf("Child process start to commuicate with socket client...\n");
			close(listenfd); /* Child process close the listen socket fd */

			while(1)
			{
				memset(buf, 0, sizeof(buf));
				rv=read(clifd, buf, sizeof(buf));
				if( rv < 0 )
				{
					printf("Read data from client sockfd[%d] failure: %s\n", clifd,strerror(errno));
					close(clifd);
					exit(0);
				}
				else if( rv == 0) 
				{
					printf("Socket[%d] get disconnected\n", clifd);
					close(clifd);
					exit(0);
				}
				else if( rv > 0 )
				{
					printf("Read %d bytes data from Server: %s\n", rv, buf);
				}

				rv=write(clifd, buf, rv);
				if(rv < 0)
				{
					printf("Write to client by sockfd[%d] failure: %s\n", clifd,strerror(errno));
					close(clifd); 
					exit(0);
				}


				p=strtok(buf,"/");
				while(p != NULL && j<3)
				{
					buftok[j]=p;
					p=strtok(NULL,"/");
					j++;
				}

				pos=strtok(buftok[1],"/");
				if(pos != NULL)
				{
					buftok[1]=pos;
				}

				if(sqlite_table_exist(TABLE_NAME)>0)
				{
					sqlite_init();
					sqlite_insert(buftok,TABLE_NAME);
				}
				else
				{
					sqlite_init();
					sqlite_create_table(TABLE_NAME);
					sqlite_insert(buftok,TABLE_NAME);
				}
			} /* Child process loop */
		} /* Child process start*/
	}
	return 0;
}



void print_usage(char *progname)
{
	printf("%s usage: \n", progname);
	printf("-p(--port): sepcify server listen port.\n");
	printf("-h(--Help): print this help information.\n"); return ;
}


void signal_stop(int signum)
{
	if(SIGUSR1 == signum)
	{
		printf("signum SIGUSR1 detected\n");
		g_sigstop = 1;
	}
	if(SIGUSR2 == signum)
	{
		printf("signum SIGUSR2 detected\n");
		g_sigstop = 1;
	}
}


void set_socket_rimit(void)
{
	struct  rlimit limit={0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur=limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);

	return;
}
