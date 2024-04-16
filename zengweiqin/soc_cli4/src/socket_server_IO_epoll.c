#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/epoll.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/resource.h>
#include <libgen.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <poll.h>
#include <ctype.h>
#include <netinet/in.h>

#define MAX_EVENTS  512
#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

int socket_server_init(char *listen_ip,int listen_port);
//static inline void msleep(unsigned long ms);
static inline void print_usage(char *progname);
void set_socket_rlimit(void);

int main(int argc,char **argv)
{
    int                 listenfd;
	int					connfd;
	int					epollfd;
    int                 events;
    int                 daemon_run=0;
    char                *progname=NULL;
    int                 opt;
    int                 found;
    int                 i,j;
    int                 serv_port=0;
    int                 rv;
    char                buf[1024];
    struct epoll_event  event;
    struct epoll_event  event_array[MAX_EVENTS];
	struct              Data
	{   
		float           temp1;
		char            time[32];
		char            num[16];
	}data;
    struct option       long_options[]=
    {
        {"daemon",no_argument,NULL,'b'},
        {"port",required_argument,NULL,'p'},
        {"help",no_argument,NULL,'h'},
        {NULL,0,NULL,0}
    };
    progname=basename(argv[0]);
    while((opt = getopt_long(argc,argv,"bp:h",long_options,NULL)) != -1)
    {
        switch(opt)
        {
            case 'b':
                daemon_run=1;
                break;
            case 'p':
                serv_port=atoi(optarg);
                break;
            case 'h':
                print_usage(progname);
                return EXIT_SUCCESS;
            default:
                break;
        }
    }

    if(!serv_port)
    {
        print_usage(progname);
        return -1;
    }

	//socket成功
	if((listenfd=socket_server_init(NULL,serv_port))<0)
	{
		printf("ERROR:%s server listen on port %d failure!\n",argv[0],serv_port);
		return -2;
	}
	printf("%s server start to listen on port %d\n",argv[0],serv_port);

	if(daemon_run)/*放置后台运行*/
	{
		daemon(0,0);
	}

    set_socket_rlimit();

	//创建epoll实例
    if((epollfd=epoll_create(MAX_EVENTS))<0)
    {
        printf("epoll_create() failure:%s\n",strerror(errno));
        return -3;
    }

    event.events=EPOLLIN|EPOLLET;//读，边缘触发
    event.data.fd=listenfd;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event)<0)//加进数组
    {
        printf("epoll add listen socket failure:%s\n",strerror(errno));
        return -4;
    }

    for(; ;)
    {
        events=epoll_wait(epollfd,event_array,MAX_EVENTS,-1);//epollfd中有多少个发生了事件，放进event_array
        if(events<0)
        {   
            printf("epoll failure:%s\n",strerror(errno));
            break;//退出循环，再次epoll—wait
        }
        else if(events==0)
        {
            printf("epoll get timeout.");
            continue;
        }

        /*events>0说明有多少个事件发生，依次遍历*/
        for(i=0;i<events;i++)
        {
            if((event_array[i].events&EPOLLERR)||(event_array[i].events&EPOLLHUP))
            {
                printf("epoll wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
                epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
                close(event_array[i].data.fd);
            }
            if(event_array[i].data.fd==listenfd)
            {
                if((connfd=accept(listenfd,(struct sockaddr *)NULL,NULL))<0)
                {
                    printf("accept new client failure:%s\n",strerror(errno));
                    continue;//连接失败继续连接
                }
                event.data.fd=connfd;
                event.events=EPOLLIN;
                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event)<0)
                {
                    printf("epoll add client socket failure:%s\n",strerror(errno));
                    close(event_array[i].data.fd);
                    continue;//再次尝试加进去
                }
                printf("epoll add new client socket[%d] ok.\n",connfd);
            }
            else//为已经连接的connfd,event_array[i].data.fd!=listenfd
            {
				memset(buf,0,sizeof(buf));

                if((rv=read(event_array[i].data.fd,buf,sizeof(buf)))<=0)
                {
                    printf("socket[%d] read failure or get disconnected.\n",event_array[i].data.fd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
                    close(event_array[i].data.fd);
                    continue;
                }
                else
                {
					memcpy(&data,buf,sizeof(data));
                    printf("socket[%d] read %d bytes data.\n",event_array[i].data.fd,rv);
					printf("序列号：%s,时间：%s,温度：%.2f\n",data.num,data.time,data.temp1);

					/*  
                    for(j=0;j<rv;j++)
                        buf[j]=toupper(buf[j]);
					*/
                    if(write(event_array[i].data.fd,buf,sizeof(buf))<0)
                    {
                        printf("socket[%d] write failure:%s\n",event_array[i].data.fd,strerror(errno));
                        epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
                        close(event_array[i].data.fd);
                    }
                }
            }   
        }
    }
ClearUp:
	close(listenfd);
	return 0;
}

int socket_server_init(char *listen_ip,int listen_port)
{
	int			        on=1;
	struct sockaddr_in	servaddr;
	int			        rv=0;
	int			        listenfd;
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Use socket() to create a TCP socket failure:%s\n",strerror(errno));
		return -1;
	}
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	memset(&servaddr,0,sizeof(servaddr)); 
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(listen_port);
	if(!listen_ip)
	{
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	else
	{
		if(inet_pton(AF_INET,listen_ip,&servaddr.sin_addr)<=0)
		{
			printf("inet_pton() set listen IP address failure%s\n",strerror(errno));
			rv=-2;
			goto ClearUp;
		}
	}
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
	{
		printf("Use bind() to bind the TCP socket failure:%s\n",strerror(errno));
		rv=-3;
		goto ClearUp;
	}
	if(listen(listenfd,13)<0)
	{
		printf("Listen the listenfd[%d] failure:%s\n",listenfd,strerror(errno));
		rv=-4;
		goto ClearUp;
	}
ClearUp:
	if(rv<0)
		close(listenfd);
	else
		rv=listenfd;
	return rv;
}

static inline void print_usage(char *progname)
{
     printf("Usage:%s [OPTION]...\n",progname);
     printf("%s is a socket server program,which used to verify client and echo bank string from it\n",progname);
     printf("\nMandatory arguments to long options are mandatory for short options too:\n");
     printf("-b[daemon] Set program running on bankground\n");
     printf("-p[port] Socket server port address\n");
     printf("-h[help] Display this help information\n");
     printf("\nExample:%s -b -p 8900\n",progname);
     return ;
}

void set_socket_rlimit(void)
{
    struct rlimit limit={0};
    getrlimit(RLIMIT_NOFILE,&limit);
    limit.rlim_cur=limit.rlim_max;
    setrlimit(RLIMIT_NOFILE,&limit);
    printf("set socket open fd max count to %ld\n",limit.rlim_max);
}
