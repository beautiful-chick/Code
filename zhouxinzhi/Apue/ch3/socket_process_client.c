/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  socket_process_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "20/02/24 15:55:09"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>

void help_printf(char *program);
int ds18b20(float *temp);

int main(int argc,char *argv[])
{
	int  							rv = -1;
	int								on = 1;
	float							temp = -1;
	char							*ip_addr = NULL;
	int 							conn_port = -1;
	int 							choo = -1;
	int 							conn_fd = -1;
	struct sockaddr_in				servaddr_in;
	char							buf[1024];
	struct option					opts[]=
	{
		{"help",no_argument,NULL,'h'},
		{"ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'i'},
		{0,0,0,0}
	};

	while((choo = getopt_long(argc,argv,"hi:p:",opts,NULL)) != -1)
	{
		switch(choo)
		{
			case 'h':
				help_printf(argv[0]);
				return 0;
			case 'i':
				ip_addr = optarg;
				break;
			case 'p':
				conn_port = atoi(optarg);
				break;
		}
	}

	if (ip_addr == NULL || conn_port < 0)
	{
		help_printf(argv[0]);
		return -1;
	}
	printf("will connect ip:%s port:%d\n",ip_addr,conn_port);

   /* if ((rv = ds18b20(&temp)) < 0)
	{
		printf("ds18b20 error:%s\n",strerror(errno));
		return -2;
	}
	printf("%.2f\n",temp);
    */

	rv = -1;
	if ( (conn_fd = socket(AF_INET,SOCK_STREAM,0))< 0)
	{
		printf("socket error%s\n",strerror(errno));
		return -3;
	}
	printf("socket_fd:%d\n",conn_fd);
   
	//setsockopt(conn_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on) );


	memset(&servaddr_in,0,sizeof(servaddr_in));
	servaddr_in.sin_family        	 = AF_INET;
	servaddr_in.sin_port 			 = htons(conn_port);
	inet_aton(ip_addr,&servaddr_in.sin_addr);
	if ((rv = connect(conn_fd,(struct sockaddr*)&servaddr_in,sizeof(servaddr_in))) < 0)
	{
		printf("bind error:%s\n",strerror(errno));
		close(conn_fd);
		return -4;
	}
	printf("connect success!\n");

	while(1)
	{
        ds18b20(&temp);
		rv = -1;
		memset(buf,0,sizeof(buf));
		snprintf(buf,sizeof(buf),"real-temp:%.2f\n",temp);
		if (write(conn_fd,buf,strlen(buf)) < 0 )
		{
			printf("error:%s\n",strerror(errno));
			continue;
		}
		memset(buf,0,sizeof(buf));
		if ((rv = read(conn_fd,buf,sizeof(buf))) <= 0 )
		{
			printf("error:%s\n",strerror(errno));
			return -7;
		}
		printf("server message:%s\n",buf);

		sleep(10);
	}

	return 0;
}

//用来打印help菜单的函数
void help_printf(char *program)
{
	printf("%s usage:\n",program);
	printf("--help(-h):help menu\n");
	printf("--ip(-i):ip adress you want connect\n");
	printf("--port(-p):port you want connect\n");
	
	return ;
}
//用来测量温度的函数
int ds18b20(float *temp)
{
	int 						fd = -1;
	int 						found = -1;
	char 						*w1_path = "/sys/bus/w1/devices/";
	char						chip_path[128];
    char 						ds_path[512];
	DIR							*dirp = NULL;
	char						*ptr = NULL;
	struct dirent 				*direntp = NULL;
    char						buf[128];

	if (temp == NULL)
	{
		printf("argument error:%s\n",strerror(errno));
		return -1;
	}
	 
	if ( (dirp = opendir(w1_path)) < 0 )  
	{
		printf("dirp error:%s\n",strerror(errno));
		return -2;
	}
	printf("opendir success:%p\n",dirp);

	while( (direntp = readdir(dirp)) != NULL)
	{
		if(strstr(direntp->d_name,"28-"))
		{ 
			strncpy(chip_path,direntp->d_name,sizeof(chip_path));
			found = 1;
		}
	}
    printf("%d\n",found);	

	if (found < 0)
	{
		printf("found error:%s\n",strerror(errno));
		return -2;
	}
	printf("found devices name:%s\n",chip_path);
    
	snprintf(ds_path,sizeof(ds_path),"%s%s/w1_slave",w1_path,chip_path);
	printf("all route:%s\n",ds_path);

	closedir(dirp);

	if ((fd = open(ds_path,O_RDONLY)) < 0)
	{
		printf("error:%s\n",strerror(errno));
		return -3;
	}
	printf("open success fd[%d]\n",fd);

	if (( read(fd,buf,sizeof(buf)) ) <= 0 ) 
	{
		printf("read error:%s\n",strerror(errno));
		close(fd);
		return -4;
	}

	if ((ptr = strstr(buf,"t=")) == NULL)
	{
		printf("error:%s\n",strerror(errno));
		close(fd);
		return -5;
	}
    
	close(fd);
	ptr += 2;

	*temp = atof(ptr)/1000;

	return 0;

}
