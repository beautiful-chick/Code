/*********************************************************************************
 *      Copyright:  (C) 2024 lingyun<lingyun>
 *                  All rights reserved.
 *
 *       Filename:  name_resolution.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/02/24)
 *         Author:  lingyun <lingyun>
 *      ChangeLog:  1, Release initial version on "16/02/24 21:50:14"
 *      
 *      //了解域名解析和反向域名解析，主要了解两个函数
 *      //分别是gethostbyname()和gethostbyaddr()
 ********************************************************************************/

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

void printf_help(char *progname)//打印使用菜单和help菜单
{
	printf("%s usage:\n",progname);
	printf("--net_name[-n]:Domain name\n");
	printf("--help[-h]:help menu\n");
	return ;
}

char** webn_toaddr(char *name)//转换域名为IP地址
{
	struct hostent 					*host_ip;
	char 							**pptr = NULL;
	char                            str[32];
	char							**rv = NULL;

	if (name == NULL)
	{
		printf("webn_toaddr argument error[%s]\n",strerror(errno));
        return NULL;
	}

	host_ip =(struct hostent *) malloc(sizeof(struct hostent));
	memset(host_ip,0,sizeof(struct hostent));//初始化hostent结构体,但是要先进行malloc不然会发生段错误。
	if ( (host_ip = gethostbyname(name)) < 0)
	{
		printf("error[%s]",strerror(errno));
		return NULL;
	}
	/*struct hostent   这里我们把hostent结构体放出来方便看 
	{
		char  *h_name;//正式的主机名称。
        char **h_aliases;//这个主机的别名。
	    int    h_addrtype;//主机名的类型。通常是AF_INET
	    int    h_length; //地址的比特长度。
	    char **h_addr_list;//从域名服务器取得的主机的地址。网络字节顺序。 　　
	}*/

	printf("official hostname:%s\n",host_ip->h_name);//打印规范名
	
	for(pptr = host_ip->h_aliases ; *pptr != NULL ; pptr++)//打印所有的别名
	{
		printf("other hostname:%s\n",*pptr);
	}

	//根据地址类型打印地址的ip地址值，注意传回来的是网络字节序的顺序
	switch(host_ip->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
			pptr = host_ip->h_addr_list;
			for(;*pptr!=NULL;pptr++)
			{
				printf("address type [%d],address long,[%d],address [%s]\n",host_ip->h_addrtype,host_ip->h_length,inet_ntop(host_ip->h_addrtype, *pptr, str, sizeof(str)));
			}
			break;

		default:
			printf("unknow address %s\n",strerror(errno));
			break;
	}

	rv = host_ip->h_addr_list; 
	return rv;
}

int main(int argc,char *argv[])
{
    int 							choo  = -1;
	char 							*webname = NULL;
	char							buf[32];
	struct sockaddr_in 				text;
	struct option					opts[]=
	{
		{"help",no_argument,NULL,'h'},
        {"net_name",required_argument,NULL,'n'},
		{0,0,0,0}
	};//选项的结构体,注意这里的最后一项是''中间添加字母，其实本质是一个int值。是ASCILL码和abcd...单词之间的关系。
	char 							**webaddr = NULL;

	while( (choo = getopt_long(argc,argv,"hn:",opts,NULL)) != -1 )
	{
		switch(choo)
		{
			case 'h'://打印help内容
				printf_help(argv[0]);
 				return 0;
			case 'n':
      			webname = optarg;//*obtarg是一个指针，会指向'选项'后的参数首地址。
				break;
		}
	}

	if (webname == NULL)//若未传参数，那么结束程序并且答应help
	{
		printf_help(argv[0]);
		return -1;
	}
	printf("webname[%s]\n",webname);//若成功那么打印获取的webname

    webaddr = webn_toaddr(webname);

	printf("%s\n",inet_ntop(AF_INET,*webaddr,buf,sizeof(buf)));


	return 0;
}


