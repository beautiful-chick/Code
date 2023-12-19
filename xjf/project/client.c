#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

int get_temperature(float *T);

void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):spceify server IP address\n");
	printf("-p(--port):spceify server port.\n");
	printf("-h(--help):printf this help information.\n");
	return;
}

int main(int argc, char **argv)
{
	float                   T=0;
	int                     ch;
	int                     port=0;
	int                     conn_fd=-1;
	int                     rv=-1;
	char                    buf[1024];
	char                   *servip=NULL;
	char                    MSG_T[32];
	struct sockaddr_in      serv_addr;
	struct option           opts[]={
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while((ch=getopt_long(argc,argv,"i:p:h",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'i':
				servip=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!servip || !port)
	{
		print_usage(argv[0]);
		return 0;
	}

	rv=get_temperature(&T);
	memset(MSG_T,0,sizeof(MSG_T));
	snprintf(MSG_T,sizeof(MSG_T),"%f",T);

	conn_fd=socket(AF_INET,SOCK_STREAM,0);
	if(conn_fd<0)
	{
		printf("Create socket failure: %s\n",strerror(errno));
		return -1;
	}
	printf("Create socket[%d] successfully!\n",conn_fd);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
	inet_aton(servip, &serv_addr.sin_addr);

	rv=connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if(rv<0)
	{
		printf("Connect to server [%s:%d] failure: %s\n",servip,port,strerror(errno));
		return -2;
	}
	printf("Connect to server[%s:%d] successfully!\n",servip,port);

	while(1)
	{
		rv=write(conn_fd,MSG_T,strlen(MSG_T));
		if(rv<0)
		{
			printf("Write data to server [%s:%d] failure: %s\n",servip,port,strerror(errno));
			return -3;
		}

		memset(buf,0,sizeof(buf));
		rv=read(conn_fd,buf,sizeof(buf));
		if(rv<0)
		{
			printf("Read data from server failure: %s\n",strerror(errno));
			return -4;
		}
		else if(0==rv)
		{
			printf("Client connect to server get disconnected\n");
			return -5;
		}
		else if(rv>0)
		{
			printf("Read %d bytes data from server: '%s'\n",rv,buf);
			return -6;
		}

	}
	close(conn_fd);

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

