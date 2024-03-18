#include<stdlib.h>
#include<libgen.h>
#include<stdio.h>
#include "client.h"
//#include"sqlite.h"
int main(int argc,char **argv)
{
	int 				rv=-1;
	char				*progname=NULL;
	int 				sockfd=-1;
	int 				port=0;
	char				*servip=NULL;
	char				buf[1024];
	struct	hostent*	host;
	int					opt=0;
	int					sleep_time=10;

	char				report_info[128];
	ReportTemp 			report_temp;
	strncpy(report_temp.serial,SERIAL,64);
	//sqlite3*			db;
	char				db_name[32]="ReportTemp";
	char				tb_name[32]="record";

#if 1
	struct	option	long_options[]={
		{"help",no_argument,NULL,'h'},
		{"servip",required_argument,NULL,'i'},
		{"domain",required_argument,NULL,'d'},
		{"port",required_argument,NULL,'p'},
		{"time",required_argument,NULL,'t'},
		{NULL,0,NULL,0}
	};
#endif
	progname=basename(argv[0]);
#if 1 
	if(argc<2)
	{
		print_usage(progname);
		return -1;
	}

	while((opt=getopt_long(argc,argv,"i: d: p: t: h::",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'i':
				{
					servip=optarg;
					printf("Server's ip: %s\n",servip);
					break;
				}
			case 'd':
				{
					host=gethostbyname(optarg);		
					servip=inet_ntoa(*((struct in_addr*)host->h_addr));
					printf("server's domain: %s to  %s\n",optarg,servip);
					break;
				}
			case 'p':
				{
					port=atoi(optarg);
					printf("Server's port %d\n",port);
					break;
				}
			case 't':
				{
					sleep_time=atoi(optarg);
					break;

				}
			case 'h':
				{
					print_usage(progname);
					return 0;
				}


		}	
	}
#endif
	while(1)
	{
		//连接服务器
		rv=m_connect(&sockfd,servip,port);
		if(rv<0)
		{
			sleep(30);
			printf("Connect to server failure:%s\n");
		}
		//存储到数据库
		
		while(1)
		{
			rv=is_socket_exceptional(&sockfd);
			if(rv>0)
			{
				printf("Connect to server[%s:%d] successfully\n",servip,port);

				//上报温度
				printf("Start to report temperture\n");
#if 1
				rv=get_info(&report_temp);
				if(rv<0)
				{
					printf("Get Temperature infomation failure:%s\n",strerror(errno));
					continue;
				}
			
				printf("Serial: %s     Time: %s      Temperature :%f\n",report_temp.serial,report_temp.time,report_temp.temperature);
#endif
				memset(report_info,0,sizeof(report_info));		
				sprintf(report_info,"Serial: %s		Temperature: %f				Time: %s\n",report_temp.serial,report_temp.temperature,report_temp.time);
				rv=write(sockfd,report_info,sizeof(report_info));
				if(rv<0)
				{
					close(sockfd);
					printf("Fail to write to server:%s\n",strerror(errno));
					//检查出错机制
					break;
				}
				printf("Report temperature successfully!\n");
				sleep(sleep_time*2);	
			}else{
				printf("Get error:%s\n",strerror(errno));
				close(sockfd);
				break;
			}
			
		}
	}
}
void print_usage(char *progname)
{
	printf("%s usage\n",progname);
	printf("[-i] | [-d] server's ip or server's domain\n");
	printf("[-p] server's port\n");
	printf("[-t] the time to report\n");
}


int get_info(ReportTemp  *report_temp)
{
	char			*ptr=NULL;
	char			buf[1024];
	int				fd=-1;
	DIR				*dirp=NULL;
	struct	dirent	*direntp=NULL;
	char			chip_sn[32];
	int				found=0;
	int				rv=0;
	
	//获取当前时间
	rv=get_time(report_temp);

	if(rv<0)
	{
		printf("Fail to get time:%s\n",strerror(errno));
		return -2;
	}

	//获取当前温度
	rv=get_temperature(report_temp);	
	if(rv<0)
	{
		printf("Fail to get temperature:%s\n",strerror(errno));
		return -3;
	}

	return rv;
}


//获取序列号
int get_serial(ReportTemp  *report_temp)
{
	char 			serial_path[64]="/../../../../proc/cpuinfo";
	char			file_name[64];
	int				rv=0;
	int				fd=-1;
	char			*ptr=NULL;
	char			buf[2048];

	if((fd=open(serial_path,O_RDONLY))<0)
	{
		printf("Open cpuinfo failure : %s\n",strerror(errno));
		return -1;
	}

	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));
	//printf("Read : %s\n",buf);
	ptr=strstr(buf,"Serial");
	if(!ptr)
	{
		printf("Can not find serial\n");
		return -2;
	}

	digit_to_str(ptr,report_temp->serial);
	
	printf("serial: %s\n",report_temp->serial);	
	close(fd);

	return rv;

}


//字符串中获取数字
void digit_to_str(char* str,char *serial)
{

	while (*str != '\0')
	{
		if (*str >= '0' && *str <= '9')
		{
			strncat(serial,str,1);
		}
		str++;
	}
	
}

//获取当前时间
int get_time(ReportTemp  *report_temp)
{
	struct			tm* local_time;
	time_t			now;
	char			time_str[20];

	now=time(NULL);
	local_time=localtime(&now);
	strftime(time_str,sizeof(time_str),"%Y-%m-%d %H:%M:%S",local_time);
	strncpy(report_temp->time,time_str,sizeof(time_str));
	return 0;
}


//获取温度

int get_temperature(ReportTemp *report_temp)
{
	int					rv=0;
	int 				fd=-1;
	char				file_name[64];
	char				buf[1024];
	char				*ptr=NULL;
	char				des[16]="28-";
	char				temp_path[64]="/sys/bus/w1/devices/";
	
	rv=find_file(temp_path,des,file_name);
	
	if(rv<0)
	{
		printf("Find file  failure: %s\n",strerror(errno));
		return -1;
	}

#if 1 
	
	strncat(temp_path,file_name,sizeof(temp_path)-strlen(temp_path));
	strncat(temp_path,"/w1_slave",sizeof(temp_path)-strlen(temp_path));


	if((fd=open(temp_path,O_RDONLY))<0)
	{
		printf("Open directory: %s failure: %s\n",temp_path,strerror(errno));
		return -2;
	}

	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));
	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Can not find the temperature\n");
		return -3;
	}

	ptr+=2;
	report_temp->temperature=atof(ptr)/1000;
	close(fd);	
#endif 
	return rv;
}



int find_file( char *path,char *des,char *file_name)
{	
	DIR				*dirp=NULL;
	struct	dirent	*direntp=NULL;
	int				found=0;
	int				rv=0;

	dirp=opendir(path);
	if(!dirp)
	{
		printf("Open the %s directory failure: %s",path,strerror(errno));
		rv= -1;
	}

	printf("Open the directory success\n");

	while(NULL!=(direntp=readdir(dirp)))
	{
		if(strstr(direntp->d_name,des))
		{
			strncpy(file_name,direntp->d_name,64);
			found=1;
		}
	}
	

	if(!found)
	{
		printf("Find temperature failure: %s\n",strerror(errno));
		rv= -2;
	}

	closedir(dirp);

	printf("%s() return\n", __func__);
	return rv;

}

int m_connect(int *sockfd,char *servip,int port)
{
	//存储到数据库中
	struct sockaddr_in	servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	
	*sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(*sockfd<0)
	{
		printf("Create a socket failure: %s\n",strerror(errno));
		return -1;
	}
	printf("Create client socket[%d] successfully!\n",*sockfd);

	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	//servaddr->sin_addr.s_addr = inet_addr(servip);
	inet_aton(servip,&servaddr.sin_addr);

	if( connect(*sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect to server [%s:%d] failure: %s\n", servip, port, strerror(errno));
		return -2;
	}
	return 0;
}

//判断网络是否异常
int is_socket_exceptional(int *sockfd)
{
	int 		error;
	socklen_t	len=sizeof(error);

	//获取socket的错误信息
	if(getsockopt(*sockfd,SOL_SOCKET,SO_ERROR,&error,&len)<0)
	{
		perror("getsockopt");
		return -1;
	}

	if(error!=0)
	{
		return -1;
	}

	//无异常
	return 1;

}
