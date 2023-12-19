#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define ID       "RPI_num1"

void gettime(char *data_time);
int get_temperature(float *T);
int socketconnected(int sockfd);
int client_init(int port, char *serv_ip);


void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):spceify server IP address\n");
	printf("-p(--port):spceify server port.\n");
	printf("-t(--interv_time) RPI temperature interval time\n");
	printf("-h(--help):printf this help information.\n");
	return;
}

int main(int argc,char *argv[])
{
	float                   T=0;
	int                     ch;
	int                     rv=-1;
	int                     port=0;
	int                     sock_fd=-1;
	int                     interv_time=0;
	char                    buf[1024];
	char                    da_time[50];
	char                   *serv_ip=NULL;
	struct sockaddr_in      serv_addr;
	struct option           opts[]={
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"interv_time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	while((ch=getopt_long(argc,argv,"i:p:t:h",opts,NULL))!=-1)
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
				interv_time=atoi(optarg);
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


	while(1)
	{
		gettime(da_time);
		if((get_temperature(&T))<0)
		{
			printf("Get temperature failure: %s\n",strerror(errno));
			continue;
		}

		memset(buf,0,sizeof(buf));
		snprintf(buf,sizeof(buf),"ID:%s time:%s and temprature:%0.3f",ID,da_time,T);

		sock_fd=client_init(port,serv_ip);
		rv=socketconnected(sock_fd);

		if(sock_fd<0)
		{
			printf("Connect server point failure: %s\n",strerror(errno));
			return -2;
		}

		if(rv>=0)
		{
                        if(write(sock_fd,buf,sizeof(buf))<0)
			{
				printf("Write data to server failure: %s\n",strerror(errno));
				close(sock_fd);
				sock_fd=-1;
				return -3;
			}
		}
		printf("\nSend message to server successfully!\n");

		sleep(interv_time);
	}

	close(sock_fd);

	return 0;

}


int client_init(int port,char *serv_ip)
{
	int                 con_fd=-1;
	int                 rv=-1;
	struct sockaddr_in  serv_addr;

	con_fd=socket(AF_INET,SOCK_STREAM,0);

	if(con_fd<0)
	{
		printf("Create socket failure: %s\n",strerror(errno));
		return -1;
	}

	printf("Create socket[%d] successfully!\n",con_fd);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
	inet_aton(serv_ip,&serv_addr.sin_addr);

	if(con_fd>=0)
	{
		rv=connect(con_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		if(rv<0)
		{
			printf("Connect to server[%s:%d] failure: %s\n",serv_ip,port,strerror(errno));
			return -2;
		}
		printf("Connect to server[%s:%d] successfully!\n",serv_ip,port);
	}
	return con_fd;
}

void gettime(char *data_time)
{
	struct tm*          st;
	time_t              timep;

	time(&timep);
	st=gmtime(&timep);
	memset(data_time,0,sizeof(data_time));
	snprintf(data_time,50,"%04d-%02d-%02d %02d:%02d:%02d",(1900+st->tm_year),(1+st->tm_mon),st->tm_mday,(st->tm_hour+8),st->tm_min,st->tm_sec);

	return;
}

int socketconnected(int sockfd)
{
	struct tcp_info   info;
	int               len=sizeof(info);

	if(sockfd<=0)
	{
		printf("Sockfd is error!\n");
		return 0;
	}
	getsockopt(sockfd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&len);
	if(info.tcpi_state == TCP_ESTABLISHED)
	{
		printf("Server connected!\n");
		return 1;
	}
	else
	{
		printf("Server disconnected!\n");
		return 0;
	}
}

int get_temperature(float *T)
{
	int               fd=-1;
	int               found=0;
	char              buf[128];
	char             *ptr=NULL;
	char              chip_sn[32];
	char              w1_path[64]="/sys/bus/w1/devices/";
	DIR              *dirp=NULL;
	struct dirent    *direntp=NULL;



	dirp=opendir(w1_path);
	if(!dirp)
	{
		printf("Open folder failure: %s\n",strerror(errno));
		return -1;
	}

	while(NULL !=(direntp=readdir(dirp)))
	{ 
		if(strstr(direntp->d_name, "28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found=1;
		}

	}

	closedir(dirp);

	if(!found)
	{
		printf("Can not find ds18b20 chipset\n");
		return -2;
	}

	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	if((fd=open(w1_path,O_RDONLY))<0)
	{
		printf("Open file failure: %s\n",strerror(errno));
		return -3;
	}

	memset(buf,0,sizeof(buf));

	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("Read data from fd=%d failure: %s\n",fd,strerror(errno));
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Can not find t= \n");
		return -5;
	}

	ptr +=2;
	*T=atof(ptr)/1000;
	printf("temprature: %f\n",*T);

	close(fd);
}
