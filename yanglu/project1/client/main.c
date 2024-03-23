/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file is socket client sample source code
 *                 
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 15:24:18"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<dirent.h>
#include<stdlib.h>
#include<getopt.h>
#include<fcntl.h>
#include <time.h>
#include<netinet/tcp.h>

int get_temperature(float *temp);
void print_usage(char *progname);
int get_dev(char *ID,int len);
int get_tm(char *localt);
int socket_alive(int fd);

int main(int argc,char ** argv)
{
	int 						connfd=-1;
	int 						k;
	int							rv=-1;
	float						temp;
	int							time;
	char						buf[1024];
	int							len=20;
	char 						Id[20];
	char						localt[64];
	struct sockaddr_in			servadrr;
	char						*servip;
	int							port;
	int							ch;

	struct option				opts[]=
	{
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
 		{"time",required_argument,NULL,'t'},
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
				time=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!servip||!port||!time)
	{
		print_usage(argv[0]);
		return 0;
	}

	connfd=socket(AF_INET,SOCK_STREAM,0);
	if(connfd<0)
	{
		printf("Create socket failure:%s",strerror(errno));
		return -1;
	}

	memset(&servadrr,0,sizeof(servadrr));
	servadrr.sin_family=AF_INET;
	servadrr.sin_port=htons(port);
	inet_aton(servip,&servadrr.sin_addr);

	if(connect(connfd,(struct sockaddr *)&servadrr,sizeof(servadrr))<0)
	{
		printf("connect to server[%s] [%d] failure:%s\n",servip,port,strerror(errno));
		return -1;
	}
	printf("connect to server[%s] [%d] successfully!\n",servip,port);
	
	while(1)
	{
		memset(buf,0,sizeof(buf));
		get_temperature(&temp);
		get_dev(Id,len);
		get_tm(localt);
		snprintf(buf,sizeof(buf),"id:%s,temperature:%.2f,time:%s",Id,temp,localt);
		k=socket_alive(connfd);
		if(k<0)//服务端断开
		{
			//写进数据库

			//重新连接
			connfd=socket(AF_INET,SOCK_STREAM,0);

			//从数据库取出数据，发送到服务端

			if(write(connfd,buf,strlen(buf)));
			{
				printf("Write data to server failure:%s",strerror(errno));
				goto CleanUp;
			}
		}
		else//服务端未断开
		{
			if(write(connfd,buf,strlen(buf))<0)
			{
				printf("Write data to server failure:%s\n",strerror(errno));
				goto CleanUp;
			}	
		}

 		memset(buf,0,sizeof(buf));
		rv=read(connfd,buf,sizeof(buf));
		if(rv<0)
		{
			printf("Read data from server failure:%s\n",strerror(errno));
			goto CleanUp;
		}

		else if(rv==0)
		{
			printf("Client connect to server get disconnected\n");
			goto CleanUp;
		}

		printf("%s\n",buf);
		sleep(time);
	}

CleanUp:
	close(connfd);

return 0;
}

void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):specify server IP address.\n");
	printf("-p(--port):specify server port.\n");
	printf("-t(--time):Sampling interval.\n");
	printf("-h(--help):print this help information.\n");
	return;
}

int get_temperature(float *temp)
{
	int					fd=-1;
	char				buf[128];
	char				*ptr=NULL;
	DIR					*dirp=NULL;
	struct dirent		*direntp=NULL;
	char				w1_path[64]="/sys/bus/w1/devices/";
	char				chip_sn[32];
	int					found=0;

	dirp=opendir(w1_path);
	if(!dirp)
	{
		printf("Open folder %s failure:%s\n",w1_path,strerror(errno));
		return -1;
	}

	while(NULL!=(direntp=readdir(dirp)))
	{
		if(strstr(direntp->d_name,"28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found=1;
		}
	}

	closedir(dirp);

	if(!found)
	{
		printf("Cannot find ds18b20 chipset\n");
		return -2;
	}

	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	if((fd=open(w1_path,O_RDONLY))<0)
	{
		printf("Open file failure:%s\n",strerror(errno));
		return -3;
	}

	memset(buf,0,sizeof(buf));
	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("Read data from fd[%d] failure:%s\n",fd,strerror(errno));
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Cannot find t= string\n");
		return -5;
	}

	ptr +=2;
	*temp=atof(ptr)/1000;
	close(fd);
	return 0;
}

//获取产品序列号
int get_dev(char *ID,int len)
{
	int sn=1;
	snprintf(ID,len,"%05d",sn);
	
}

//获取当地时间
int get_tm(char *localt)
{
	time_t	seconds;
	struct tm *local;

	time(&seconds);

	local = localtime(&seconds);

	snprintf(localt,64,"%d-%d-%d %d:%d:%d\n",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	return 0;
}

//检查服务端是否断线
int socket_alive(int fd)
{
	struct tcp_info		info;
	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&sizeof(info));
	if((info.tcpi_state==TCP_ESTABLISHED))
	{
		printf("The server is not disconnected\n");
		return 0;
	}
	else
	{
		printf("The server is disconnected\n");
		return -1;
	}
}

/*  int socket_set_kalive(int fd)
{
	int 		alive=1;
	int 		idle=10;
	int			t=3;
	int			cnt=5;

	if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&alive,sizeof(alive))!=0)
	{
		printf("Set keepalive error:%s\n",strerror(errno));
		return -1;
	}

	if(setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,&idle,sizeof(idle))!=0)
	{
		printf("Set keepalive idle failure:%s\n",strerror(errno));
		return -2;
	}

	if(setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,&t,sizeof(t))!=0)
	{
		printf("Set keepalive intv error:%s\n",strerror(errno));
		return -3;
	}

	if(setsockopt(fd,SOL_TCP,TCP_KEEPCNT,&cnt,sizeof(cnt))!=0)
	{
		printf("Set keepalive cnt error:%s\n",strerror(errno));
		return -4;
	}


	return 0;
}*/

