#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
 #include <unistd.h>
#define BUFSIZE 1024
#define MSG_STR "hello world\n"

int main(int argc,char **argv)
{
	int  		fd = -1;
	int		rv = -1;
	char		buf[BUFSIZE];
	fd = open("test.txt",O_RDWR|O_CREAT|O_TRUNC,0666);
	
	if(fd<0)
	{
		perror("Open/Create file test.txt failure\n ");
		return 0;
	}
	printf("Open file returned file descriptor [%d]\n",fd);
	
	if((rv = write(fd,MSG_STR,strlen(MSG_STR)))<0)
	{
		printf("write %d bytes into file failure: %s\n",rv,strerror(errno));
		goto cleanup;
	}
	memset (buf,0,sizeof(buf));
	if((rv = (read(fd,buf,sizeof(buf)))<0))
	printf("Read %d bytes data from file :%s\n",rv,buf);
cleanup:
	close (fd);
	return 0;

}

