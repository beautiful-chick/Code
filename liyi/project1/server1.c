#include<libgen.h>
#include<stdio.h>
#include<unistd.h>
#include"server.h"

int  main(int argc,char **argv)
{
	int 			clifd=0;
	int 			rv=-1;
	int 			port=0;
	socklen_t		len;
	int 			opt=0;
	struct sockaddr_in	cliaddr;
	char			buf[1024];
	char			*progname=NULL;
	int 			daemon_run=0;
	int				epoll_fd=-1;
	int				listen_fd=-1;


	struct 	option	long_options[]=
	{
					{"daemon",no_argument,NULL,'d'},
					{"port",required_argument,NULL,'p'},
					{"help",no_argument,NULL,'h'},
					{NULL,0,NULL,0}
	};
	
	progname=basename(argv[0]);
	while((opt=getopt_long(argc,argv,"p: h::",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'p':
				{
					port=atoi(optarg);
					break;
				}
			case 'h':
				{
					print_usage(progname);
					break;
				}
			case 'd':
				{
					daemon_run=1;
				}
			default:
				break;
		}

	}

	if(argc<2)
	{
		print_usage(progname);
		return -1;
	}

	set_socket_rlimit();


	if((listen_fd=socket_server_init(NULL,port))<0)
	{
		printf("Fail to init server:%s\n",strerror(errno));
		return -3;
	}
	printf("Create listen socket:%d successfully!\n",listen_fd);

//设置在后台运行
	if(daemon_run==1)
	{
		daemon(0,0);
	}
	
	//创建一个epoll 里面可以检查最大的文件描述符个数未MAX_EVENTS
	if((epoll_fd=epoll_create(MAX_EVENTS))<0)
	{
			printf("Fail to create epoll:%s\n",strerror(errno));
			return -4;
	}
	printf("Create epoll_fd:%d successfully!\n",epoll_fd);

	//如果返回的rv>0 那么返回的是epoll_fd,否则出现错误
	rv=accept_client(&epoll_fd,&listen_fd);
	printf("Epoll finished\n");
	close(epoll_fd);

	return 0;

}

void print_usage(char *progname)
{
	printf("%s usage : [-p] server's port\n",progname);
}


int socket_server_init(char *listen_ip,int listen_port)
{
	int 			listen_fd=-1;
	int				rv=0;	
	int 			on=1;
	struct sockaddr_in	servaddr;


	if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Fail to create socket:%s\n",strerror(errno));
		rv=-1;
		goto CleanUp;
	}

	//使得socket可以重新使用，修补当服务器重新启动时，地址已经存在得bug;
	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_port=htons(listen_port);
	if(listen_ip==NULL)
	{
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	else
	{
		if(inet_pton(AF_INET,listen_ip,&servaddr.sin_addr)<=0)
		{
			printf("Set client ip failure:%s\n",strerror(errno));
			rv=-2;
			goto CleanUp;
		}
	}
	

	if(bind(listen_fd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		printf("Bind failure:%s!\n",strerror(errno));
		rv=-3;
		goto CleanUp;
	}
	printf("Bind on server socket successfully!\n");

	if(listen(listen_fd,BACKLOG)<0)
	{
		printf("Fail to listen:%s\n",strerror(errno));
		rv=-4;
		goto CleanUp;
	}
	printf("Start to listen socket\n");
	goto CleanUp;

CleanUp:
	if(rv<0)
	{
		return rv;
	}
	return listen_fd;

}
void set_socket_rlimit(void)
{
	struct	rlimit limit={0};

	//设置从内核获得的资源   软资源 和硬资源  软资源的范围在0到硬资源的范围内
	//RLIMIT_NOFILE 代表的是当前进程的文件描述符大一的数
	
	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur=limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);

	printf("Set socket open fd max count to %d\n",limit.rlim_max);
	
}

int accept_client(int* epoll_fd,int* listen_fd)
{
	struct epoll_event		event;
	struct epoll_event		event_array[MAX_EVENTS];
	int						rv=0;
	int						events;
	char					buf[1024];
	int						client_fd;

	event.events=EPOLLIN;
	event.data.fd=*listen_fd;
	if(epoll_ctl(*epoll_fd,EPOLL_CTL_ADD,*listen_fd,&event)<0)
	{
		printf("Epoll add listen socket failure:%s\n",strerror(errno));
		rv=-1;
		goto CleanUp;
	}

	while(1)
	{	
		//第一步等待事件发生 返回处于就绪态的文件描述符,在timeout时间内没有事件发生就返回0,错误返回-1.events 是已经发生的文件描述符的事件的掩码,timeout=1说明peoll一直阻塞到有事件发生为止
		printf("listen_fd:%d\n",*listen_fd);
		memset(buf,0,sizeof(buf));
		char		ch[32]="Hello world";
		strncpy(buf,ch,sizeof(buf));
		rv=write(*listen_fd,buf,sizeof(buf));
		if(rv<0)
		{
				printf("Fail to write to server:%s\n",strerror(errno));
				continue;
		}
		printf("Writing successfully\n");


		printf("Waiting for events...\n");

		events=epoll_wait(*epoll_fd,event_array,MAX_EVENTS,-1);
		printf("Events:%d\n",events);
		if(events<0)
		{
			printf("Epoll failure:%s\n",strerror(errno));
			break;
		}
		else if(0==events)
		{
			printf("Epoll get timeout!\n");
			continue;
		}
		
		printf("Start to do something\n");
		for(int i=0;i<events;i++)
		{
			 printf("For\n");
			 //如果是ERROR 或者HUP的事件
			 if((event_array[i].events&EPOLLERR)||(event_array[i].events&EPOLLHUP))
			 {
			 	printf("Epoll wait get error or client get disconnected on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
				close(event_array[i].data.fd);
			 }
			 printf("Not error\n");
			 if(*listen_fd==event_array[i].data.fd)
			 {
					printf("Listen fd\n");
			 		if((client_fd=accept(*listen_fd,(struct sockaddr*)NULL,NULL))<0)
					{
						printf("Fail to accept new client socket:%s\n",strerror(errno));
						epoll_ctl(*epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						continue;
					}
					printf("Creat a client sockct:%d successfully\n",client_fd);
					event.events=EPOLLIN;
					event.data.fd=client_fd;
					epoll_ctl(*epoll_fd,EPOLL_CTL_ADD,client_fd,&event);
			 }
			 else
			 {
					memset(buf,0,sizeof(buf));
					printf("Client fd\n");
					if((rv=read(event_array[i].data.fd,buf,sizeof(buf)))<0)
					{
						printf("Fail to read:%s !\n",strerror(errno));
						epoll_ctl(*epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,&event);
						continue;
					}
					printf("Read %d bytes :%s from client\n",rv,buf);
			 		
					if((rv=write(event_array[i].data.fd,buf,sizeof(buf)))<0)
					{
						printf("Fail to write to server:%s\n",strerror(errno));
						epoll_ctl(*epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						continue;
					}
					printf("Writing successfully\n");


			 }
		}
	}
	goto CleanUp;

CleanUp:
	close(*listen_fd);
	if(rv<0)
	{
		return rv;
	}
	return *epoll_fd;
}
