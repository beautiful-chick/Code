#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sqlite3.h>
#include <libgen.h>

#include "main.h"
#include "sqlite3.h"
#include "database.h"
#include "ds18b20.h"
#include "get_time.h"
#include "socket_client.h"

#define TABLE_NAME       "RPI_num1"

int                       g_sigstop = 0;

int main(int argc,char *argv[])
{
	float                   T=0;
	int                     ch;
	int                     rv=-1;
	int                     port=0;
	int                     count=0;
	int                     connfd=-1;
	int                     daemon_run=0;
	int                     con_status=1;
	int                     set_time=0;
	char                    time_buffer[50];
	char                    count_buffer[20];
	char                    temp_buffer[50];
	char                    da_time[50];
	char                    *buftok[4];
	double                  current_time=0;
	double                  localtime=0;
	char                    *programe_name;
	char                    *serv_ip=NULL;
	struct                  timeval tv;
	struct                  sigaction sigact;
	struct                  sigaction sigign;
	struct                  sockaddr_in serv_addr;
	struct                  option opts[]={
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"interv_time",required_argument,NULL,'t'},
		{"daemon",no_argument,NULL,'d'},
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	while((ch=getopt_long(argc,argv,"i:p:t:dh",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'i':
				serv_ip=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
	         	case 't':
				set_time=atoi(optarg);
				break;
			case 'd':
				daemon_run=1;
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!serv_ip || !port)
	{
		print_usage(argv[0]);
		return -1;
	}

	if(daemon_run)
	{
		if(daemon(1,1)<0)
		{
			printf("Running daemon failure:%s\n",strerror(errno));
			return 0;
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
	if(sqlite_table_exist(TABLE_NAME)==0)
	{
		sqlite_create_table(TABLE_NAME);
	}
	count=table_row_count(TABLE_NAME);

	gettimeofday(&tv,NULL);
	current_time=tv.tv_sec;

	while(!g_sigstop)
	{
		sleep(set_time);
		gettimeofday(&tv,NULL);
		localtime=tv.tv_sec;

		connfd=client_init(port,serv_ip);                                  
		if(connfd<0)
		{                                            
			printf("sock_fd failure:%s\n",strerror(errno));
			con_status=0;
		}

		if(socketconnected(connfd)==0)                          
		{                                                       
			con_status=0;                   
		}

		if((localtime-current_time)>=set_time)
		{
			if((rv=get_temperature(&T))<0)
			{
				printf("Get temperature failure: %s\n",strerror(errno));
				return -2;	
			}

			 localtime=gettime(da_time);
			 current_time=localtime;
			 memset(time_buffer,0,sizeof(time_buffer));
			 snprintf(time_buffer,sizeof(time_buffer),"%s",da_time);
			 memset(count_buffer,0,sizeof(count_buffer));
			 snprintf(count_buffer,sizeof(count_buffer),"%d",(count+1));
			 memset(temp_buffer,0,sizeof(temp_buffer));
			 snprintf(temp_buffer,sizeof(temp_buffer),"%.3f",T);
			 buftok[0]=count_buffer;
			 buftok[1]="RPI_num1";
			 buftok[2]=time_buffer;
			 buftok[3]=temp_buffer;
			 buftok[4]="\n";

			if(con_status)
			{
				if(count>0)
				{
					while(count)
					{
						sqlite_init();
						sendata2(connfd,TABLE_NAME,count);
						sleep(1);
						sqlite_delete(count,TABLE_NAME);
						count--;
					}
					printf("The table data is null!\n");
				}

				else
				{
					sendata(connfd,buftok);
				}
			}
			else
			{
				connfd=client_init(port,serv_ip);
				if(connfd < 0)
				{
					printf("Client connect server failure1:%s\n",strerror(errno));
					sqlite_init();
					count=table_row_count(TABLE_NAME);

					con_status=0;
					sqlite_insert(buftok,TABLE_NAME);
					count++;
					close(connfd);
				}
				else
				{
					con_status=1;
					if(sendata(connfd,buftok)<0)
					{
						sqlite_init();
						count=table_row_count(TABLE_NAME);
						sqlite_insert(buftok,TABLE_NAME);
						count++;
						close(connfd);
					}
				}
			}	
		}
	}
	return 0;
}




void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):spceify server IP address\n");
	printf("-p(--port):spceify server port.\n");
	printf("-t(--set_time):RPI temperature interval time\n");
	printf("-d(--daemon):run background\n");
	printf("-h(--help):printf this help information.\n");
	return;
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



