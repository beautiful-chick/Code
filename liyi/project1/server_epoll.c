#include"server_epoll.h"	
int main(int argc,char **argv)
{
		int 			listen_fd=-1;
		int 			connfd=-1;
		int 			rv=-1;
		int			opt=-1;
		char			*progname;
		char			buf[1024];
		int			maxfd=-1;
		int 			daemon_run=0;
		int 			i=0,j=0;
		int			serv_port=-1;
		char 			*serv_ip;
		int			epoll_fd=-1;
		struct	epoll_event	event;
		int			events;
		struct	epoll_event	event_array[MAX_EVENTS];
		struct	option		long_options[]={
			{"port",required_argument,NULL,'p'},
			{"help",no_argument,NULL,'h'},
			{"daemon",no_argument,NULL,'d'},
			{NULL,0,NULL,0}
		};

		progname=basename(argv[0]);

		while((opt=getopt_long(argc,argv,"bp:h",long_options,NULL))!=-1)
		{
			switch(opt)
			{
				case 'h':
					printf_usage(argv[0]);
					break;
				case 'p':
					serv_port=atoi(optarg);
					break;

				case 'b':
					daemon_run=1;
					break;

				default:
					break;
			}

		}

		if(!serv_port)
		{
			printf_usage(argv[0]);
			return -1;
		}


		//设置最大触发是事件的数量
		set_socket_rlimit();


		//创建listen_fd
		listen_fd=socket_server_init(NULL,serv_port);
		if(listen_fd<0)
		{
			printf("ERROR: %s server listen on port %d failure:%s\n",argv[0],serv_port,strerror(errno));
			return -2;
		}
		printf("%s server start to listen on port %d \n",argv[0],serv_port);

		if(daemon_run)
		{
			daemon(0,0);
		}

		//创建epoll
		epoll_fd=epoll_create(MAX_EVENTS);
		if(epoll_fd<0)
		{
			printf("Epoll create failure: %s\n",strerror(errno));
			return -3;
		}

		printf("Epoll_fd create successfully!\n");

		event.events=EPOLLIN;
		event.data.fd=listen_fd;

		if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&event)<0)
		{
			printf("ADD listen_fd to epoll_fd failure: %s\n",strerror(errno));
			return -4;
		}

		while(1)
		{
			//程序在这里阻塞
			events=epoll_wait(epoll_fd,event_array,MAX_EVENTS,-1);
			if(events<0)
			{
				printf("No events :%s\n",strerror(errno));
				break;
			}
			else if(0==events)
			{
				printf("Get timeout!\n");
				break;
			}

			for(int i=0;i<events;i++)
			{
				if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP))
				{
					printf("failure: %s\n",strerror(errno));
					epoll_ctl(epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,&event);
					close(event_array[i].data.fd);
					continue;
				}
				
				//listen_fd触发事件
				if(listen_fd==event_array[i].data.fd)
				{
					connfd=accept(listen_fd,(struct sockaddr *)NULL,NULL);
					if(connfd<0)
					{
						printf("Accept client socket failure: %s\n",strerror(errno));
						close(event_array[i].data.fd);
						continue;
					}
					event.events=EPOLLIN;
					event.data.fd=connfd;

					if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connfd,&event)<0)
					{
						printf("Epoll add connfd failure: %s\n",strerror(errno));
						close(event_array[i].data.fd);
						continue;
					}
					printf("Epoll add client socket[%d] successfully!\n",event_array[i].data.fd);


				}
				else
				{
					if((is_socket_exceptional(&event_array[i].data.fd))<0)
					{
						prinf("Get error:%s\n",strerror(errno));
						//继续for循环
						close(event_array[i].data.fd);
						continue;
					}

					memset(buf, 0, sizeof(buf));
					//connfd触发事件
					if(rv=read(event_array[i].data.fd,buf,sizeof(buf))<0)
					{
						printf("Read data failure: %s\n",strerror(errno));
						epoll_ctl(epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,&event);
						close(event_array[i].data.fd);
						continue;
					}
					printf("Read %d bytes data : %s \n",rv,buf);


#if 0
					for(int j=0;i<sizeof(buf);j++)
					{
						buf[j]=toupper(buf[j]);
					}
					if(write(event_array[i].data.fd,buf,sizeof(buf))<0)
					{
						printf("Write to client socket[%d] failure: %s\n",event_array[i].data.fd,strerror(errno));
						epoll_ctl(epoll_fd,EPOLL_CTL_DEL,event_array[i].data.fd,&event);
						close(event_array[i].data.fd);
						continue;
					}
					printf("Write to client socket[%d] successfully!\n",event_array[i].data.fd);
#endif 

				}//else
			}


		}



CleanUP:
		close(listen_fd);
		return 0;
}


int socket_server_init(char *listen_ip,int listen_port)
{
	struct sockaddr_in	serv_addr;
	int			on=1;
	int			rv=1;
	int 			listen_fd;
	//服务器端 首先创建socket  ipv4 tcp 0
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd<0)
	{
		printf("Create a socket failure: %s!\n",strerror(errno));
		rv=-1;
	}
	printf("Create a socket[%d] successfully!\n",listen_fd);
	//第二步，重置服务器端的数据 为什么这里求大小用的是指针呢？
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(listen_port);

	if(!listen_ip)
	{
		serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	else
	{
		printf("Can not to connect specify client ip\n");
		rv=-3;
	}
	//第三步步 bind
	if(bind(listen_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		printf("Use %d to bind the TCP socket  failure: %s!\n",listen_fd,strerror(errno));
		rv -4;
		goto  CleanUp;
	}
	//第四步 服务器监听
	if(listen(listen_fd,BACKLOG)<0)
	{
		printf("Use bind() to bind the TCP socket failure: %s\n",strerror(errno));
		rv -3;
		goto CleanUp;
	}

CleanUp:
	if(rv>0)
		rv=listen_fd;
	else
		close(listen_fd);
	return rv;

}
static inline void printf_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n",progname);
	printf("--d[daemon] set program running on background...\n");
	printf("--p[port] set server port...\n");
	printf("--h[help] displey this help information\n");
}


void set_socket_rlimit(void)
{
	struct rlimit limit={0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur=limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);

	printf("Set socket open fd max count to %ld\n",limit.rlim_max);
}

int is_socket_exceptional(int* sockfd)
{
	int				error;
	socklen_t	 	len=sizeof(error);

	if(getsockopt(*sockfd,SOL_SOCKET,SO_ERROR,&error,&len)<0)
	{
		//获取信息错误
		perror("getsockopt\n");
		return -1;
	}
	if(error!=0)
	{
		//异常
		return -1;
	}

	return 1;
}
