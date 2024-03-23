/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  Project_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "20/03/24 12:37:34"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define		TIMEOUT 5

#define	SERVER_IP			"127.0.0.1"
#define SERVER_PORT			2345
//struct tcp_info info;

struct DS18B20_DATA
{
	char            *dev_time;
	float           dev_temp;
	char            dev_buf[1024];
}data;

int main(int argc,char **argv)
{


	/*DS18B20采集温度*/
	int							fd;
	int							fd2;
	char						buf[1024];
	char						buf2[1024];
	char						*ptr;
	float						temp;
	DIR							*dirp = NULL;
	char        				w1_path[64] = "/sys/bus/w1/devices/";//定义写文件夹的指针
	char						w2_path[64] = "/sys/bus/w1/devices/";
	char                		ds18b20_path[164];
	struct dirent				*direntp = NULL;//定义读文件夹的指针
	char        				chip_sn[32];
	int         				found = 0;
	time_t						timer;
	struct tm					*Now;
	char						*now_time;
	struct	DS18B20_DATA		*p;
/*	struct DS18B20_DATA{
		char		*dev_time;
		float		dev_temp;
		char		dev_buf[1024];

		
	}data;*/
   	p = &data; 

	/*打开文件夹*/
	dirp = opendir(w1_path);

	if( !dirp )
	{
		printf("Open the floder %s failure : %s\n",w1_path,strerror(errno));
		return -1;
	}
		
	/*读取文件夹中内容*/

	while( NULL != (direntp = readdir(dirp)) )
	{
		/*查找存储温度的文件*/

		if(strstr(direntp -> d_name,"28-"))
		{
			strncpy(chip_sn,direntp -> d_name,sizeof(chip_sn));
			found = 1;
		}
	}

	closedir(dirp);

	/*如果找不到就退出*/
	
	if( !found )
	{
		printf("can not find ds18b20 chipset\n");
		return -2;
	}

	/*获取全路径到ds18b20_path中去*/
	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));
	//printf("w1_path : %s\n ",w1_path);
	strncat(w2_path,chip_sn,sizeof(w2_path)-strlen(w2_path));
	strncat(w2_path,"/name",sizeof(w2_path)-strlen(w2_path));


	while(1)	{
	/*打开文件*/

	fd = open(w1_path,O_RDONLY);

	if( fd < 0 )
	{
		printf("Open the failure : %s\n",strerror(errno));
		return -1;
	}



	/*将读取的数据写入栈中*/

	memset(buf,0,sizeof(buf));

	if( read(fd,buf,sizeof(buf)) < 0 )
	{
		printf("Read data from fd = %d failure:%s\n",fd,strerror(errno));
		return -2;
	}

	//printf("buf:%s\n",buf);
	ptr = strstr(buf,"t=");

	/* 判断buf中的值是否为空*/

	if(NULL == ptr)   
	{
		printf("Can not find t = String\n");
		return -1;


	}
	
	ptr += 2;

	temp = atof(ptr)/1000;//字符串转化成浮点数

	/*获取设备号*/
	//strncat(w2_path,chip_sn,sizeof(w2_path)-strlen(w2_path));
	//strncat(w2_path,"/name",sizeof(w2_path)-strlen(w2_path));
	//printf("%s\n",w2_path);
	
	fd2 = open(w2_path,O_RDONLY);
	if( fd2 < 0 )
	{
		printf("open the file about name failure : %s\n",strerror(errno));
		return -1;
	}
	memset(buf2,0,sizeof(buf2));
	if( read(fd2,buf2,sizeof(buf2)) < 0 )
	{
		printf("get devices number failure: %s\n",strerror(errno));
		return -2;
	}
	printf("buf2: %s\n",buf2);


	/*获取当前时间*/
	time( &timer );
	Now = localtime(&timer);
	now_time = asctime(Now);
	//printf("Now time is %s\n",asctime(Now));

	//printf("time : %s,temperature:%.2f,devices'number:%s\n",now_time,temp,buf2);
	memset(data.dev_buf,0,sizeof(data.dev_buf));
	strcpy(data.dev_buf,buf2);
	data.dev_time = now_time;
	data.dev_temp = temp;


	printf("time : %s || device number : %s || temperature : %.2f\n",data.dev_time,data.dev_buf,data.dev_temp);

	sleep(TIMEOUT);
	}
	printf("%s %s %.2f\n",data.dev_time,data.dev_buf,data.dev_temp);
	close(fd);
}

	
	int							conn_fd = -1;
	int							rv = -1;
	char						sock_buf[1024];
	struct sockaddr_in			serv_addr;
	char						snd_buf[1024] = {0};
	int 						error = 0;
	socklen_t					len = sizeof(error);
	memcpy(snd_buf, &data, sizeof(data));
	printf("%s\n",snd_buf);

	conn_fd = socket(AF_INET,SOCK_STREAM,0);
	printf("start creating socket\n");

	if( conn_fd < 0 )
	{
		printf("create socket failure : %s\n",strerror(errno));
		return -1;
	}
	

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_IP,&serv_addr.sin_addr);
	printf("Set ok\n");
	if( connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 )
	{
		printf("Connect to server[%s:%d] failure : %s\n",SERVER_IP,SERVER_PORT,strerror(errno));
		
		return -1;

	}
	

	printf("1\n");
	/*断线重连*/

	/*判断是否断线*/

	/* 判断是否断线*/
	//getsockopt(conn_fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);

  

	if( getsockopt(conn_fd,SOL_SOCKET,SO_ERROR,&error,&len) < 0 )
	{
		goto cleanup;
	}
	else 
	{

		if(error != 0 )
		{
			printf("disconnect\n");
		}
		else
	
		{ 
		/*未断线，开始发送数据*/
	
		if( write(conn_fd,snd_buf,strlen(snd_buf)) < 0 )
		{
			printf("Write data to server[%s:%d] failure : %s\n",strerror(errno));
			goto cleanup;
	}

		memset(sock_buf,0,sizeof(sock_buf));
		rv = read(conn_fd,sock_buf,sizeof(sock_buf));
		if( rv < 0 )
		{
			printf("Read data from server failure: %s\n",strerror(errno));
			goto cleanup;
		}
		else if( rv == 0 )
		{
			printf("Client connect to server get disconnect\n");
		}
		printf("Read %d bytes data from server:'%s'\n",rv,sock_buf);

		}
	if( write(conn_fd,snd_buf,strlen(snd_buf)) < 0 )
	{
		printf("Write data to server[%s:%d] failure : %s\n",strerror(errno));
		goto cleanup;
	}

	memset(sock_buf,0,sizeof(sock_buf));
	rv = read(conn_fd,sock_buf,sizeof(sock_buf));
	if( rv < 0 )
	{
		printf("Read data from server failure: %s\n",strerror(errno));
		goto cleanup;
	}
	else if( rv == 0 )
	{
		printf("Client connect to server get disconnect\n");
	}
	}
	printf("Read %d bytes data from server:'%s'\n",rv,sock_buf);

cleanup:
	close(conn_fd);



	



	return 0;

}







int get_temperature(){
	

	int					fd;
	char				buf[1024];
	char				*ptr;
	float				temp;
	DIR					*dirp = NULL;
	char				w1_path[64] = "/sys/bus/w1/devices/";//定义写文件夹的指针

	char				ds18b20_path[164];
	struct dirent		*direntp = NULL;
	char				chip_sn[32];
	int					found = 0;
	/*打开文件夹*/
	
	dirp = opendir(w1_path);
	if( !dirp )
	{
		printf("Open the floder failure : %s\n",strerror(errno));
		return -1;
	}

	/*读取文件夹中内容*/

	while( NULL != (direntp = readdir(dirp)) )
	{
		/*查找存储温度的文件*/
		if( strstr(direntp -> d_name,"28-") )
		{
			strncpy(chip_sn,direntp -> d_name,sizeof(chip_sn));
			found = 1;
		}
	}
	closedir(dirp);

	if( !found )
	{
		printf("can not find ds18b20 chipset\n");
		return -2;
	}

	/*获取全路径到ds18b20_path中去*/
	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));
	
	while(1)
	{
		fd = open(w1_path,O_RDONLY);
		if( fd < 0 )
		{
			printf("Open the file failure : %s\n",strerror(errno));
			return -1;
		}

		memset(buf,0,sizeof(buf));

		if(read(fd,buf,sizeof(buf)) < 0 )
		{
			printf("Read data from fd = %d failure : %s\n",fd,strerror(errno));
			return -2;
		}

		ptr = strstr("buf",t=);

		/*判断buf是否为空*/

		if( NULL == ptr )
		{
			printf("Can not find t = String\n");
			return -1;
		}

		ptr += 2;

		temp = atof(ptr)/1000;
	}
	return 0;

}




char* get_name(){
	int			fd2;
	char		buf2[1024];
	char		w2_path[64] = "/sys/bus/w1/devices/";
	char		chip_sn1[32];


	strncat(w2_path,chip_sn1,sizeof(w2_path)-strlen(w2_path));
	strncat(w2_path,"/name",sizeof(w2_path)-strlen(w2_path));
	fd2 = open(w2_path,O_RDONLY);
	if( fd2 < 0 )
	{
		printf("Open the file about name failure : %s\n",strerror(errno));
		return -1;
	}
	memset(buf2,0,sizeof(buf2));
	if( read(fd2,buf2,sizeof(buf2)) < 0 )
	{
		printf("get devices number failure : %s\n",strerror(errno));
		return -2;
	}

	return buf2;
}



char *get_time(){

	time( &timer );
	Now = localtime( &timer );
	now_time = asctime(Now);
	return now_time;
}







int internet_socket(){




	int							conn_fd = -1;
	int							rv = -1;
	char						sock_buf[1024];
	struct sockaddr_in			serv_addr;
	char						snd_buf[1024] = {0};
	conn_fd = socket(AF_INET,SOCK_STREAM,0);

	if( conn_fd < 0 )
	{
		printf("Create socket failure : %s\n",strerror(errno));
		return -1;
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_IP&serv_addr.sin_addr);
	if( connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("Connect to server[%s:%d] failure : %s\n",SERVER_IP,SERVER_PORT,strerror(errno));
		return -1;
	}

	if( write(conn_fd,snd_buf,strlen(snd_buf)) < 0 )
	{
		printf("Wirte data to server[%s:%d] failure : %s\n",SERVER_IP,SERVER_PORT,strerror(errno));
		goto cleanup;
	}

	memset(sock_buf,0,sizeof(sock_buf));

	rv = read(conn_fd,sock_buf,sizeof(sock_buf));

	if( rv < 0 )
	{
		printf("Read data from server failure : %s\n",strerror(errno));
		goto cleanup;
	}
	else if( 0 == rv)
	{
		printf("Client connect to server failure : %s\n",strerror(errno));
	}
	printf("Read %d bytes data from server : '%s'\n",rv,sock_buf);
cleanup:
	close(conn_fd);
	return 0;
}



