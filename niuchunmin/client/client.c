/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file a client for the first project.
 *                 
 *        Version:  1.0.0(16/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "16/03/24 16:06:21"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <time.h>

#define  MSG_STR "Hello,I am ready to connect with you!"
#define  TEMP "temperature"

void print_usage(char *progname);
int  get_temperature(float *temp);
int  get_devid(char *id,int len);
void get_time(char *time_buf);
int main (int argc, char **argv)
{
	int                  conn_fd=-1;
	int                  rv=-1;
	struct sockaddr_in   serv_addr;
	int                  port;
	char                *servip=NULL;
	char                 buf[1024];
	int                  ch=-1;
	int                  temp_time=10;
	float                temp;
	char                 id[16];
	int                  len=16;
	char                 time_buf[128];

	struct option       opts[]={
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"temp_time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while((ch=getopt_long(argc,argv,"i:p:t:h",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'i':
				servip=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 't':
				temp_time=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!servip||!port||!temp_time)
	{
		print_usage(argv[0]);
		return 0;
	}
	while(1)
	{
	conn_fd = socket(AF_INET,SOCK_STREAM,0);
	if(conn_fd<0)
	{
		printf("create socket failure:\n",strerror(errno));
		return -1;
	}
	printf("Create client[%d] successfully!\n",conn_fd);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_aton(servip,&serv_addr.sin_addr);

	if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		printf ("Connect to server [%s:%d] failure:%s\n",servip,port,strerror(errno));
		goto cleanUp;
	}
	
	printf ("Connect to server [%s:%d] successfully\n",servip,port);

/*  memset(buf,0,sizeof(buf));
	rv =read(conn_fd,buf,sizeof(buf));
	if(rv<0)
	{

		printf ("Read data from server failure:%s\n",strerror(errno));
		goto cleanUp;
	}
	else if(0==rv)
	{

		printf ("Client connect to server get disconnected:%s\n",strerror(errno));
		goto cleanUp;
	}


	printf ("Read %d bytes data from server:%s\n",rv,buf);
*/

	/* Acquire devece id */
	if((rv=get_devid(id,len))<0)
	{

		printf ("Get device id failure:%s\n",strerror(errno));
		goto cleanUp;
	}
	printf("Device id:%s\n",id);
	/* Acquire current time */
	get_time(time_buf);
//	printf("current time:%s\n",time_buf);
	/* Acquire temperature */
	if((rv=get_temperature(&temp))<0)
	{
		printf("Acquire temperature failure:%s\n",strerror(errno));
		goto cleanUp;
	}
	
	printf ("debug00:temp:%.2f\n",temp);

	memset(buf,0,sizeof(buf));
	snprintf(buf,sizeof(buf),"%s:%.2f",TEMP,temp);

	if(write(conn_fd,buf,strlen(buf))<0)
	{

		printf ("Write to server failure:%s\n",strerror(errno));
		goto cleanUp;
	}

	printf ("Update %s successfully!\n",buf);

	

	sleep(temp_time);
cleanUp:
	close(conn_fd);

	}

	return 0;


	
} 

void print_usage(char *progname)
{
	printf("%s:usage:\n",progname);
	printf("-i(--ipaddr):specify server IP address.\n");
	printf("-p(--port):specify server port.\n");
	printf ("-t(--time):temp upload time\n");
	printf("-h(--help):print this help information.\n");

	return ;
}

int get_temperature(float *temp)
{
	int                 fd=-1;
	char                buf[128];
	char               *ptr=NULL;
	DIR                *dirp=NULL;
	struct dirent      *direntp=NULL;
	char                w1_path[64]="/sys/bus/w1/devices/";
	char                chip_sn[32];
	int                 found=0;
	int                 rv=10;

	dirp=opendir(w1_path);
	if(!dirp)
	{
		printf("Open floder %s failure:%S\n",w1_path,strerror(errno));
		rv=-1;
		goto cleanup;
	}

	while(NULL != (direntp=readdir(dirp)))
	{
		if(strstr(direntp->d_name,"28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found=1;
		}
	}
	closedir(dirp);
    dirp=NULL;

	if(!found)
	{

		printf ("Cannot find ds18b20 chipset\n");
		rv=-2;
		goto cleanup;
	}
	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	fd=open(w1_path,O_RDONLY);
	if(fd<0)
	{
		printf ("Open file failure:%s\n",strerror(errno));
		rv=-3;
		goto cleanup;
	}

	memset(buf,0,sizeof(buf));

	if(read(fd,buf,sizeof(buf))<0)
	{

		printf ("Read file failure:%s\n",strerror(errno));
		rv=-4;
		goto cleanup;
	}

	ptr = strstr(buf,"t=");
	if(!ptr)
	{
		
		printf ("Can not find string t=\n");
		rv =-5;
		goto cleanup;
	}
	ptr += 2;
	*temp=atof(ptr)/1000;
	
	printf ("temperature:\n");
	close(fd);
cleanup:
	if(fd)
		close(fd);
	if(!dirp)
		closedir(dirp);

	return rv;
}

int  get_devid(char *id,int len)
{
	int          sn=1;
	int          rv=0;

	if(!id||!len)
	{

		printf ("Argument error:%s\n",strerror(errno));
		rv=-1;
	}
	snprintf(id,len,"DEVID%03d",sn);
	return rv;
}

void  get_time(char *time_buf)
{
	time_t        timep;
	struct tm    *p;
	time(&timep);
	p=localtime(&timep);

	memset(time_buf,0,sizeof(time_buf));

//	printf ("%d\n",sizeof(time_buf));
	snprintf(time_buf,128,"%d/%d/%d --%d:%d:%d",1900+p->tm_year,1+p->tm_mon,p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec);

	printf("%s\n",time_buf);
	return ;
//	strncpy(time_buf,asctime(gmtime(&timep)),sizeof(time_buf));

}
