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
float 	get_temperature();
char 	*get_name();
char	*get_time();
int 	internet_socket();
int		re_connect();
char	buf2[1024];
char	*now_time;
int		conn_fd = -1;
#define	SERVER_IP			"127.0.0.1"
#define SERVER_PORT			2345

struct DS18B20_DATA
{
	char            d_time[64];
	float           d_temp;
	char            d_name[64];
}data;

int main(int argc,char **argv)
{
	float 		dev_temp;
	char		*dev_name;
	char		*dev_time;
	dev_temp = get_temperature();
	dev_name = get_name();
	dev_time = get_time();
//	printf("%s\n",dev_name);
//	printf("%.2f\n",dev_temp);
//	printf("%s\n",dev_time);
	snprintf(data.d_name,32,dev_name);
	snprintf(data.d_time,32,dev_time);
	data.d_temp = dev_temp;
	printf("%s,%s,%.2f\n",data.d_time,data.d_name,data.d_temp);
	internet_socket(data);
	int re_connect();
	return 0;


}

float get_temperature(){
	

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

	ptr = strstr(buf,"t=");

	/*判断buf是否为空*/

	if( NULL == ptr )
	{
		printf("Can not find t = String\n");
		return -1;
	}
	ptr += 2;
	temp = atof(ptr)/1000;
	return temp;
}
char *get_name(){
	int				fd2;
	char			w2_path[64] = "/sys/bus/w1/devices/";
	char			chip_sn1[32];
	DIR				*dirp1 = NULL;
	struct dirent	*direntp1 = NULL;
	int				found1 = 0;
	dirp1 = opendir(w2_path);
	if( !dirp1 )
	{
		printf("Open the floder failure : %s\n",strerror(errno));
		return NULL;
	}

	while( NULL != (direntp1 = readdir(dirp1)))
	{
		if(strstr(direntp1 -> d_name,"28-"))
		{
			strncpy(chip_sn1,direntp1->d_name,sizeof(chip_sn1));
			found1 = 1;
		}
	}

	closedir(dirp1);
	if(!found1)
	{
		printf("Can not find name chipset\n");
	}
	strncat(w2_path,chip_sn1,sizeof(w2_path)-strlen(w2_path));
	strncat(w2_path,"/name",sizeof(w2_path)-strlen(w2_path));
	fd2 = open(w2_path,O_RDONLY);
	if( fd2 < 0 )
	{
		printf("Open the file about name failure : %s\n",strerror(errno));
		return NULL;
	}
	memset(buf2,0,sizeof(buf2));
	if( read(fd2,buf2,sizeof(buf2)) < 0 )
	{
		printf("get devices number failure : %s\n",strerror(errno));
		return NULL;
	}
	//printf("%s\n",buf2);
	return buf2;
}

char *get_time(){
	time_t			timer;
	struct tm		*Now;
	char			*now_time;

	time( &timer );
	Now = localtime( &timer );
	now_time = asctime(Now);
	return now_time;
}

int internet_socket(struct DS18B20_DATA data){
//	int							conn_fd = -1;
	int							rv = -1;
	char						sock_buf[1024];
	struct sockaddr_in			serv_addr;
	char						snd_buf[2048] = {0};
	conn_fd = socket(AF_INET,SOCK_STREAM,0);
	printf("data.d_temp:%.2f\n",data.d_temp);
	snprintf(snd_buf,2048,"%s,%s,%.2f",data.d_time,data.d_name,data.d_temp);	
	

	if( conn_fd < 0 )
	{
		printf("Create socket failure : %s\n",strerror(errno));
		return -1;
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_IP,&serv_addr.sin_addr);
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
	printf("write ok\n");
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


 int re_connect()
{

	int					error = 0;
	socklen_t 			len = sizeof(error);
	getsockopt(conn_fd,SOL_SOCKET,SO_ERROR,&error,&len);
	/*连接失败：继续获取数据，将数据存储在数据库中*/
		if(error != 0 )
		{
			printf("disconnect\n");

			internet_socket(data);
		}
		/*重连成功：将数据库中的数据上传，并删除数据库中的数据*/
		else
		{
			return 0;
		}
	
}
