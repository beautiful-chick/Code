/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(25/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "25/03/24 20:05:44"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <sqlite3.h>
#include <signal.h>
#include "sqliteall.h"
#include "ds18b20.h"
#include "client.h"

static inline void print_usage(char *progname);
void handle_pipe(int sig);
void get_time(char buf[], int time_size);

int main(int argc, char **argv)
{
	int                     conn_fd = -1;
	int                     rv = -1;
	int                     serv_port = 0;
	int                     daemon_run = 0;
	int                     opt;
	int                     stime=0;
	float                   temperature = 0;
	char                   *progname = NULL;
	char                    buffer1[32];
	char                    buffer2[64];
	char                    rpi[32];
	char                   *serv_ip = NULL;
	struct tcp_info         info;
	int                     len=sizeof(info);
	sqlite3                *db=NULL;
	char                   *sql=NULL;
	int                     flat1=0;              
	int                     i;
	struct option              long_options[] =
	{
		{"daemon", no_argument, NULL, 'b'},
		{"ip", required_argument, NULL, 'i'},
		{"port", required_argument, NULL, 'p'},
		{"time", required_argument, NULL, 't'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	progname = basename(argv[0]);
	while((opt = getopt_long(argc, argv, "bi:p:t:h", long_options, NULL)) != -1)
	{
		switch(opt)
		{
			case'b':
				daemon_run = 1;
				break;
			case'i':
				serv_ip = optarg;
				break;
			case'p':
				serv_port = atoi(optarg);
				break;
			case't':
				stime =atoi(optarg);
				break;
			case'h':
				print_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;
		}
	}

	if( !serv_port || !serv_ip || !stime)
	{
		print_usage(progname);
		return -1;
	}

	if(daemon_run)
	{
		daemon(0, 0);
	}

	create_statement(db, sql);
	delete_statement(db, sql);
 	signal(SIGPIPE, handle_pipe);
	while(1)
	{

		getsockopt(conn_fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);

		if(info.tcpi_state != TCP_ESTABLISHED)  
		{
			if(	connect_server(serv_ip, serv_port,&conn_fd) < 0)
			{
				dbg_print("\nServer disconnected, next start sqilte\n");
				flat1++;

				if((rv=get_temperature(&temperature, rpi)) < 0)
				{
					dbg_print("get temperrature failure: %d\n", rv);
					close(conn_fd);
					continue;
				}

				get_time(buffer1,sizeof(buffer1));
				memset(buffer2, 0, sizeof(buffer2));
				snprintf(buffer2,sizeof(buffer2),"%s %s %.3f", rpi, buffer1, temperature);
				insert_statement(db, sql, rpi, buffer1, &temperature);

				dbg_print("%s\n", buffer2);
				sleep(stime);
			}
		}

		if(info.tcpi_state == TCP_ESTABLISHED)
		{
			if(flat1>0)
			{
				client_select_statement(db, sql);
				for(i=0;i<flat1;i++)
				{       
					printf("\n**send data: %s\n", da[i].array);
					printf("\n");
					sleep(1);

					if( write(conn_fd, da[i].array, strlen(da[i].array)) < 0)
					{
						dbg_print("Write data to server [%s:%d] failure: %s\n", serv_ip, serv_port, strerror(errno));
						close(conn_fd);
						continue;
					}
				}
				flat1=0;
				delete_statement(db, sql);
			}
			
			if((rv=get_temperature(&temperature, rpi)) < 0)
			{
				dbg_print("get temperrature failure: %d\n", rv);
				close(conn_fd);
				continue;
			}
			get_time(buffer1,sizeof(buffer1));
			memset(buffer2, 0, sizeof(buffer2));
			snprintf(buffer2,sizeof(buffer2),"%s %s %.3f", rpi, buffer1, temperature);
			printf("\nsend data:%s\n",buffer2);

			if( write(conn_fd, buffer2, strlen(buffer2)) < 0)
			{
				dbg_print("Write data to server [%s:%d] failure: %s\n", serv_ip, serv_port, strerror(errno));
				close(conn_fd);
				continue;
			}
			sleep(stime);
		}

	}
}

static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n",progname);

	printf(" %s is a socket server program, which used to verify client and echo back string from it\n",progname);

	printf("\nMandatroy arguments to long options are mandatroy for short options too:\n");

	printf(" -b[daemon ] set program running on background\n");
	printf(" -i[ip ] Socket server ip address\n");
	printf(" -p[port ] Socket server port address\n");
	printf(" _h[help ] Display this help information\n");

	printf("\nExample: %s -b -i 192.168.1.1 -p 8900 -t 5\n",progname);

	return ;
}

void get_time(char buf[], int time_size)
{
	struct tm              *current_tm;
	time_t                  time_now;

	time(&time_now);
	current_tm = localtime(&time_now);
	memset(buf, 0, time_size);
	snprintf(buf, time_size, "%02i-%02i-%02i %02i:%02i:%02i", 
			current_tm->tm_year+1900, current_tm->tm_mon+1,
			current_tm->tm_mday, current_tm->tm_hour, 
			current_tm->tm_min,current_tm->tm_sec);

	return ;
}

void handle_pipe(int sig)
{
	if(sig == SIGPIPE)
	{
		;
	}
}
		
