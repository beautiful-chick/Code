/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client_one1.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 00:35:43"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <libgen.h>
#include <pthread.h>
#include <ctype.h>
#include "ds18b20.h"//
#include <fcntl.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8889
#define MSG_STR "Hello, Unix Network Program World!"
#define RETRY_INTERVAL 5  // 重连间隔（秒）
void ds18b20_get_temperature(float *temp);

void print_usage( char *progname)
{
	printf("%s usage:  n",progname);
	printf("_i( -- ipaddr): specify server Ip address\n");
	printf("-p(--port): specify server PORT\n");
	printf("-h(--help): print this help information\n");
	printf("-d(--time): print this time information\n");
	return;
}
int main(int argc, char** argv)  
{ 
        int                     conn_fd ; 
        int                     rv = -1; 
        char                    buf[1024]; 
        struct 					sockaddr_in      serv_addr; 
		char					*server_ip = NULL;
		int 					ch;
		unsigned short			port=0;	
		char					buf_temper[1024];
		float					temp;

		time_t					time_now;
		char					buf_time[100];
		int						*temp_time;
		fd_set 					rdset;
	    int                     maxfd; 
		int						ret;
		struct 					timeval timeout;
		int 					i,j;
		timeout.tv_sec = 5;
        timeout.tv_usec = 0;
		int flags;
		struct  option          opts[]  ={
			    {"ipaddr", required_argument, NULL, 'i'},
                {"port",required_argument,NULL,'p'},
                {"time",required_argument,NULL,'d'},
                {"help",no_argument,NULL,'h'},
                {NULL,0,NULL,0}
        };
    

	
 	   printf("start paser argements\n");

       while( (ch=getopt_long(argc,argv,"i:p:d:h",opts,NULL)) != -1)
        {
                switch(ch)
               {
						case 'i':
								server_ip=optarg;
								break;
                        case 'p':
                                port=atoi(optarg);
                                break;
                        
						case 'd':
                  		    	*temp_time =atoi(optarg);
								break;
                        case 'h':
							 print_usage(argv[0]); 
							return EXIT_SUCCESS;
						 default:
							 break;

                }


        }
		printf("时间间隔%d\n",*temp_time);			
        if(!port||!server_ip)
        {
                print_usage(argv[0]);
                return 0;
        }
      
	
        conn_fd = socket(AF_INET, SOCK_STREAM, 0);  

     
		if (conn_fd < 0) 
        { 
                printf("create socket failure: %s\n", strerror(errno)); 
                return -1; 
        } 
		printf("create socket [%d] ok\n",conn_fd);
		memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        inet_aton(server_ip, &serv_addr.sin_addr);

        if (connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
          {
                  printf("connect to  server [%s:%d] failure: %s\n", SERVER_IP, port, strerror(errno));
                  return 0;
          }

        printf("Connected to server...\n");  
     
	
 
 		while(1)
		{
			printf("ddddd\n");
			FD_ZERO(&rdset);  
			FD_SET(conn_fd, &rdset);  
		

       //  使用select检测socket是否可读（即是否连接）  
        	ret = select(conn_fd+1, NULL, &rdset, NULL, &timeout);
					
		    if (ret == -1) {  
		        perror("select");  
		        goto cleanup;  
		        return -1;  
			
		    }
			else if (ret == 0) {  
		        // 超时，连接失败  
		        goto cleanup;  
		        return -1;  
		    }

			else {  
		        int error;  
		    socklen_t len = sizeof(error);  
		  
		        // 检查socket的错误状态  
		        if (getsockopt(conn_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {  
		            perror("getsockopt");  
		            goto cleanup;  
		            return -1;  
		        }  
		  
		      if (error) {  
		            // 连接失败  
		        	goto cleanup;  
		            return -1;  
		        }  
		    } 
			 

printf("aaaaaa\n");
        // 如果socket不可读，说明连接已断开，尝试重连  
        if (!FD_ISSET(conn_fd, &rdset)) {  
            printf("Connection lost, retrying...\n");
            close(conn_fd); 
            sleep(2);
		 


            // 尝试重新连接  
            if ((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
                perror("ERROR opening socket");  
  
            if (connect(conn_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {  
				perror("ERROR connecting");  
                continue;  
            }  
 printf("bbbbbbb\n");
  
            printf("Reconnected to server...\n");  
            continue;  
		}          
			memset(buf_time, 0, sizeof(buf_time));
			
		    time(&time_now);
		    char* buf_time = ctime(&time_now);
 printf("cccccccccc\n");

			printf("receive: listen port[%d]\n", port);
		    memset(buf_temper, 0, sizeof(buf_temper));
			ds18b20_get_temperature(&temp);
			sprintf(buf_temper,"temperature:%f,time:%s",temp,buf_time);
		printf("ddddddddd\n");

        if(write(conn_fd, buf_temper, strlen(buf_temper)) < 0)
        {
                printf("Write data to server [%s:%d] failure: %s\n", SERVER_IP, port, strerror(errno));
                goto cleanup;
        }
        
		
	    
		
        		memset(buf, 0, sizeof(buf));
        		rv=read(conn_fd, buf, sizeof(buf));
        	if(rv < 0)
        {
                printf("Read data from server failure: %s\n", strerror(errno));
                goto cleanup;
        }
        	else if( 0 == rv )
                                                                             

        {
                printf("Client connect to server get disconnected\n");
                goto cleanup;
        }   
		     

        printf("Read %d bytes data from server: '%s'\n", rv, buf);
		
  				

       sleep(*temp_time);
}	
 cleanup:
        close(conn_fd);
        
}

