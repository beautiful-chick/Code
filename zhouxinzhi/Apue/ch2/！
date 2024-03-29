/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  socket_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(18/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "18/02/24 20:28:27"
 *                 每十秒上报一次温湿度的客户端
 ********************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>

int ds18b20(float *temp);
void help_printf(char *program);
int socket_client(int *sock_fd,char *listen_ip,int *listen_port);

int main(int argc, char *argv[])
{
	int							sock_fd = -1;
	int 						listen_port = -1;
	char 						*listen_ip = NULL;
	int 						choo = -1;
	float 						temp = -1;
   	char						buf[1024];
	int 						rv = -1;
	struct option				opts[]=
	{
		{"help",no_argument,NULL,'h'},
		{"port",required_argument,NULL,'p'},
		{"ip",required_argument,NULL,'i'},
		{0,0,0,0}
	};

	while( (choo = getopt_long(argc,argv,"hp:i:",opts,NULL)) != -1 ) 
	{
		switch(choo)
		{
			case 'h':
				help_printf(argv[0]);
				return 0;
			case 'p':
				listen_port = atoi(optarg);
				break;
			case 'i':
				listen_ip = optarg;
				break;
		}
	}

	//printf("will connect ip:%s port:%d\n",listen_ip,listen_port);
    if ( (listen_port == 0) || (listen_ip == NULL) )
	{
		help_printf(argv[0]);
		return -1;
	}
	printf("will connect ip:%s port:%d\n",listen_ip,listen_port);

	//ds18b20(&temp);
	//printf("real-time temperature:%.2f'c\n",temp);
    //sock过程开始

	if (socket_client(&sock_fd,listen_ip,&listen_port) < 0)
	{
		printf("error:%s\n",strerror(errno));
		return -2;
	}

	while(1)
	{   
		ds18b20(&temp);
		printf("real-time temperature:%.2f'c\n",temp);

		memset(buf,0,sizeof(buf));
		snprintf(buf,sizeof(buf),"temp:%.2f",temp);
        if ((rv = write(sock_fd,buf,strlen(buf))) < 0)
		{
			printf("write error\n");
			close(sock_fd);
			return -3;
		}

		if ((rv = read(sock_fd,buf,sizeof(buf))) <= 0)
		{
			printf("error:%s\n",strerror(errno));
			close(sock_fd);
			return -4;
		}

		printf("server message:%s\n",buf);

		sleep(10);
	}

	return 0;
}

int socket_client(int *sock_fd,char *listen_ip,int *listen_port)
{
	int 						client_fd = -1;
	struct sockaddr_in 			clieaddr_in;

	if ((*sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("socket error:%s\n",strerror(errno));
		return -1;
	}
	printf("client_fd %d\n",*sock_fd);

	memset(&clieaddr_in,0,sizeof(clieaddr_in));
	clieaddr_in.sin_family    	= AF_INET;
	clieaddr_in.sin_port 		= htons(*listen_port);
	inet_aton(listen_ip,&clieaddr_in.sin_addr);
	printf("listen_port:%d\n",*listen_port);
	printf("listen_ip:%s\n",listen_ip);

	if (connect(*sock_fd,(struct sockaddr*)&clieaddr_in,sizeof(clieaddr_in)))
	{
		printf("connect error:%s\n",strerror(errno));
		close(*sock_fd);
		return -3;
	}
	printf("connect success!\n");
	return 0;

}

void help_printf(char* program)
{
	printf(" %s usage:\n",program);
	printf("--help(-h):help menu\n");
	printf("--port(-p):port\n");
	printf("--ip(-i);ip address\n");

	return ;

}

int ds18b20(float *temp)
{
	int 						fd = -1;
	int 						found_var = -1;
	char 						*w1_path = "/sys/bus/w1/devices/";
    char						chip_path[128];
	char						ds_path[256];
	DIR  						*dirp = NULL;
	struct dirent 				*direntp = NULL;
	char 						buf[128];
    char						*ptr = NULL;

	if (NULL == temp)
	{
		printf("arguments error:%s\n",strerror(errno));
		return -1;
	}

    if ( (dirp = opendir(w1_path)) == NULL )
	{
		printf("dirp nofound error:%s\n",strerror(errno));
		return -2;
	}
	printf("open dir %s success,DIR address:%p\n",w1_path,dirp);

	while ( (direntp = readdir(dirp)) != NULL )
	{
		if( strstr(direntp->d_name,"28-") )
		{
			strncpy(chip_path,direntp->d_name,sizeof(chip_path));  //chip_path = *direntp->d_name;不行，为什么编译会报错
			found_var = 1;
		}
	}
	
	if (found_var < 0)
	{
		printf("nofound dir 28-...\n");
		closedir(dirp);
		return -3;
	}
	printf("success found [%s]\n",chip_path);

	closedir(dirp);

	snprintf(ds_path,sizeof(ds_path),"%s%s/w1_slave",w1_path,chip_path);
	printf("ds18b20 devices route:%s\n",ds_path);
	
	if ((fd = open(ds_path,O_RDONLY)) < 0)
	{
		printf("open error:%s\n",strerror(errno));
		return -4;
	}

	memset(buf,0,sizeof(buf));
	if (read(fd,buf,sizeof(buf)) <= 0)
	{
		printf("read error:%s\n",strerror(errno));
		return -5;
	}

	close(fd);

	if ((ptr = strstr(buf,"t=")) == NULL)
	{
		printf("ptr error:%s\n",strerror(errno));
		close(fd);
		return -6;
	}
	
	ptr +=2;
    
	*temp = atof(ptr)/1000;

	return 0;
}
