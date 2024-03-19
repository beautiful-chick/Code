/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  fork_exec.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(28/02/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "28/02/24 16:18:05"
 *                 
 *****************************************************************/ 
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
//标准输出重定向的文件，/tmp路径是Linux系统在内存里做的一个文件系统，放在这里不用写硬盘程序运行快
#define	TMP_FILE		"/tmp/ifconfig.log"

int main(int argc,char **argv)
{
	pid_t		pid;
	int			fd;
	char		buf[1024];
	int			rv;
	char		*ptr;
	FILE		*fp;
	char		*ip_start;
	char		*ip_end;
	char		ipaddr[16];
	/*父进程打开这个文件，子进程将会继承父进程打开的这个文件描述符，这样父子进程可以通过各自的文件描述符访问同一个文件*/

	if(( fd = open(TMP_FILE,O_RDWR|O_CREAT|O_TRUNC,0644)) < 0)
	{
		printf("Redirect standard output to file failure : %s\n",strerror(errno));
		return -1;
	}

	/*父进程创建子进程*/

	pid = fork();

	if(pid < 0)
	{
		printf("fork() create child process failure : %s\n",strerror(errno));
		return -1;
	}

	else if(pid == 0)
	{
		printf("Child process start excute ifconfig program\n");

		/*子进程会继承父进程打开的文件描述符，此时子进程重定向标准输出到父进程所打开的文件里*/

		dup2(fd,STDOUT_FILENO);

		/*下面这句execl()函数是让子进程开始执行带参数的ifconfig命令：ifconfig eth0
		 * execl()会导致子进程彻底丢掉父进程的文本段、数据段，并加载/sbin/ifconfig这个程序的文本段、数据段重新建立内存空间。
		 * execl()函数的第一个参数是所要执行的程序的路径，ifconfig命令（程序）的路径是/sbin/ifconfig；
		 *接下来的参数是命令及其相关选项、参数，每个命令、选项 都用双引号括起来，并以NULL结尾*/

		/*ifconfig eth0 命令在执行时会将命令的执行的结果输出到标准输出上，而这时子进程已经重定向标准输出到文件中去了，所以ifconfig命令的打印结果会输出到文件中去，这样父进程就能从文件中读到子程序执行命令的结果*/

		execl("/sbin/ifconfig","ifconfig","eth0",NULL);

		/*execl()函数并不会返回，因为他去执行另外一个程序，如果execl()返回了 说明函数调用出错*/

		printf("Child process excute another program,will not return here.Return here means execl() error\n");
		return -1;
	}
	else
	{
		/*父进程等待3s，让子进程先执行*/
		sleep(3);
	}

	/*子进程因为调用了execl()，他会丢掉父进程的文本段，所以子进程不会执行到这里，只有父进程执行下面的代码*/

	memset(buf,0,sizeof(buf));
	/*父进程此时是读不到内容的，这时因为子进程往文件里写内容时，已经将文件偏移量移到文件尾*/
	rv = read(fd,buf,sizeof(buf));
	printf("Read %d bytes data dierectly read after child process write\n",rv);
	/*父进程如果要读则需要将文件偏移量设置到文件头才能读到内容*/

	memset(buf,0,sizeof(buf));
	lseek(fd,0,SEEK_SET);
	rv = read(fd,buf,sizeof(buf));
	printf("Read %d bytes data after lseek :\n %s",rv,buf);

	/*如果使用read()读的话，一下子就读N多个字节进buffer，但有时我们需要一行一行的读取文件的内容，这是可以用fdopen()函数将文件描述符fd转成文件流fp*/
	fp = fdopen(fd,"r");

	fseek(fp,0,SEEK_SET);//重新设置文件偏移量到文件头
	while(fgets(buf,sizeof(buf),fp))
	{
		/*
		 * 包含IP地址的那一行包含有netmask关键字，如果在该行中找到该关键字就可以从这里面解析出IP地址了；
		 * inet 192.168.2.17 netmask 255.255.255.0 broadcast 192.168.2.255
		 * inet6 fe80::ba27:ebff:fee1:95c3 prefixlen 64 scopeid 0x20<link>*/

		if(strstr(buf,"netmask"))
		{
			/*查找inet关键字 inet关键字后面就是IP地址*/
			ptr = strstr(buf,"inet");
			if(!ptr)
			{
				break;
			}
			ptr += strlen("inet");
			/*inet 关键字后面是空白符，我们不确定是空格还是Tab，所以接下来使用isblank()函数判断如果自复式空白符就向下跳过*/
			while(isblank(*ptr))
				ptr++;

			//跳过空白符后跟着的就是IP地址的起始字符
			ip_start = ptr;

			/*IP地址后面又是空白符，跳过所有非空白部分 即IP地址部分*/

			while(!isblank(*ptr))
				ptr++;

			//第一个空白符的地址也就是IP地址终止的字符位置
			ip_end = ptr;

			/*使用memcpy()函数将IP地址copy到存放IP地址的buffer中，其中ip_end-ip_start就是IP地址长度，ip_start就是IP地址的起始位置*/

			memset(ipaddr,0,sizeof(ipaddr));
			memcpy(ipaddr,ip_start,ip_end-ip_start);
			break;



		}
	

	}
	printf("Parser and get IP address : %s\n",ipaddr);

	fclose(fp);
	unlink(TMP_FILE);

	return 0;

}

