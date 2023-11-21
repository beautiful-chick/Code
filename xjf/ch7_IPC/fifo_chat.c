#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>

#define FIFO_FILE1 ".fifo_chat1"
#define FIFO_FILE2 ".fifo_chat2"

int                 g_stop = 0;
void sig_pipe(int signum)
{
	if(SIGPIPE == signum)
	{
		printf("get pipe broken signal and let programe exit\n");
		g_stop = 1;
	}
}
int main(int argc, char **argv)
{
	int         fdr_fifo;
	int         fdw_fifo;
	int         rv;
	fd_set      rdset;
	char        buf[1024];
	int         mode = 0;
	if( argc != 2 )
	{
		printf("Usage: %s [0/1]\n", basename(argv[0]));
		printf("This chat program need run twice, 1st time run with [0] and 2nd time with [1]\n");		   
		return -1;
       	}
	mode = atoi(argv[1]);

	/* 管道是一种半双工的通信方式，如果要实现两个进程间的双向通信则需要两个管道，即两个管道分别作为两个进程的读端和写端 */
	
	if( access(FIFO_FILE1 , F_OK) )
	{
		printf("FIFO file \"%s\" not exist and create it now\n", FIFO_FILE1);
		mkfifo(FIFO_FILE1, 0666);
	}
	if( access(FIFO_FILE2 , F_OK) )
	{
		printf("FIFO file \"%s\" not exist and create it now\n", FIFO_FILE2);
		mkfifo(FIFO_FILE2, 0666);
	}
	signal(SIGPIPE, sig_pipe);
	if( 0 == mode )
	{
		/* 这里以只读模式打开命名管道FIFO_FILE1的读端，默认是阻塞模式；如果命名管道的写端被不打开则open()将会一直阻塞,
		 * 所以另外一个进程必须首先以写模式打开该文件FIFO_FILE1，否则会出现死锁 */

		printf("start open '%s' for read and it will blocked untill write endpoint opened...\n",FIFO_FILE1);
		if( (fdr_fifo=open(FIFO_FILE1, O_RDONLY)) < 0 )
		{
			printf("Open fifo[%s] for chat read endpoint failure: %s\n", FIFO_FILE1, strerror(errno));
			return -1;
		}
		printf("start open '%s' for write...\n", FIFO_FILE2);
		if( (fdw_fifo=open(FIFO_FILE2, O_WRONLY)) < 0 )
		{
			printf("Open fifo[%s] for chat write endpoint failure: %s\n", FIFO_FILE2, strerror(errno));
			return -1;
		}
	}
	else
	{
		/* 这里以只写模式打开命名管道FIFO_FILE1的写端，默认是阻塞模式；如果命名管道的读端被不打开则open()将会一直阻塞，
		 * 因为前一个进程是先以读模式打开该管道文件的读端，所以这里必须先以写模式打开该文件的写端，否则会出现死锁 */

		printf("start open '%s' for write and it will blocked untill read endpoint opened...\n",FIFO_FILE1);
	       	if( (fdw_fifo=open(FIFO_FILE1, O_WRONLY)) < 0 )
		{
			printf("Open fifo[%s] for chat write endpoint failure: %s\n", FIFO_FILE1, strerror(errno));
			return -1;
		}
		printf("start open '%s' for read...\n", FIFO_FILE2);
		if( (fdr_fifo=open(FIFO_FILE2, O_RDONLY)) < 0 )
		{
			printf("Open fifo[%s] for chat read endpoint failure: %s\n", FIFO_FILE2, strerror(errno));
			return -1;
		}
	}
	printf("start chating with another program now, please input message now: \n");
	while( !g_stop )
	{
		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO, &rdset);
		FD_SET(fdr_fifo, &rdset);

		/* select多路复用监听标准输入和作为输入的命名管道读端 */
		rv = select(fdr_fifo+1, &rdset, NULL, NULL, NULL); 
		if( rv <= 0 )
		{
			printf("Select get timeout or error: %s\n", strerror(errno));
			continue;
		}
		/* 如果是作为输入的命名管道上有数据到来则从管道上读入数据并打印到标注输出上 */

		if( FD_ISSET(fdr_fifo, &rdset) )
		{
			memset(buf, 0, sizeof(buf));
			rv=read(fdr_fifo, buf, sizeof(buf));

			if( rv < 0)
			{
				printf("read data from FIFO get errorr: %s\n", strerror(errno));
				break;
			}
			else if( 0==rv ) /* 如果从管道上读到字节数为0，说明管道的写端已关闭 */
		       	{
				printf("Another side of FIFO get closed and program will exit now\n");
			       	break;
			}
			printf("<-- %s", buf);
		}
		/* 如果标准输入上有数据到来，则从标准输入上读入数据后，将数据写入到作为输出的命名管道上给另外一个进程 */
		if( FD_ISSET(STDIN_FILENO, &rdset) )
		{
			memset(buf, 0, sizeof(buf));
			fgets(buf, sizeof(buf), stdin);
      			write(fdw_fifo, buf, strlen(buf));
     		}
	}
}

