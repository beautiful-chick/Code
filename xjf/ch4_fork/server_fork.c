#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

#define BACKLOG               13
#define MSG_STR              "You have successfully connected server!"

void print_usage(char *progname)
{
	 printf("%s usage: \n", progname);
	 printf("-p(--port): sepcify server listen port.\n");
	 printf("-h(--Help): print this help information.\n");
	 return ;
}
int main(int argc, char **argv)
{
	 int                  listen_fd = -1;
	 int                  client_fd = -1;
	 int                  rv = -1;
	 struct sockaddr_in   serv_addr;
     	 struct sockaddr_in   cli_addr;
	 socklen_t            len;
	 int                  port = 0;
	// char                *servip=NULL;
	 char                 buf[1024];
	 int                  ch;
	 int                  on = 1;
	 pid_t                pid;
	 struct option        opts[] = {
		 {"port", required_argument, NULL, 'p'},
		 {"help", no_argument, NULL, 'h'},
		 {NULL, 0, NULL, 0}
     	 };

	 while( (ch=getopt_long(argc, argv, "p:h", opts, NULL)) != -1 )
	 {
		 switch(ch)
		 {
			 case 'p':
				 port=atoi(optarg);
				 break;
			 case 'h':
				 print_usage(argv[0]);
				 return 0;
		 }
	 }

	 if( !port )
	 {
		 print_usage(argv[0]);
		 return 0;
	 }
	 listen_fd=socket(AF_INET, SOCK_STREAM, 0);
	 if(listen_fd < 0)
	 {
		 printf("Create socket failure: %s\n", strerror(errno));
		 return -1;
	 }
	 printf("Create socket[%d] successfully!\n", listen_fd);

	 setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	 memset(&serv_addr, 0, sizeof(serv_addr));
	 serv_addr.sin_family=AF_INET;
	 serv_addr.sin_port = htons(port);
	 serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* listen all the IP address on this host */
	 rv=bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	 if(rv < 0)
      	 {
	     	 printf("Socket[%d] bind on port[%d] failure: %s\n", listen_fd, port, strerror(errno));
	    	 return -2;
   	 }
    	 listen(listen_fd, BACKLOG);
  	 printf("Start to listen on port [%d]\n", port);

	 while(1)
	 {
       		 printf("Start accept new client incoming...\n");
      		 client_fd=accept(listen_fd, (struct sockaddr *)&cli_addr, &len);
     		 if(client_fd < 0)
    		 {
	    		 printf("Accept new client failure: %s\n", strerror(errno));
	  		 continue;
 		 }
		 printf("Accept new client[%s:%d] successfully\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		 pid = fork();
		 if( pid < 0 )
		 {
    			 printf("fork() create child process failure: %s\n", strerror(errno));
   			 close(client_fd);
  			 continue;
		 }
		 else if( pid > 0 )
		 {
			 /* Parent process close client fd and goes to accept new socket client again */
			 close(client_fd);
			 continue;
		 }
		 else if ( 0 == pid )
		 {
			 int i;
			 printf("Child process start to commuicate with socket client...\n");
			 close(listen_fd); /* Child process close the listen socket fd */
			 while(1)
			 {
				 memset(buf, 0, sizeof(buf));
				 rv=read(client_fd, buf, sizeof(buf));
				 if( rv < 0 )
				 {
					 printf("Read data from client sockfd[%d] failure: %s\n", client_fd, strerror(errno));
					 close(client_fd);
					 exit(0);			    
				 }
   				 else if( rv == 0) 
				 {
					 printf("Socket[%d] get disconnected\n", client_fd);
					 close(client_fd);
					 exit(0);
				 }
				 else if( rv > 0 )
				 {
					 printf("Read %d bytes data from Server: %s\n", rv, buf);
				 }
				 /* convert letter from lowercase to uppercase */
				 for(i=0; i<rv; i++)
				 {
					 buf[i]=toupper(buf[i]);
				 }
				 rv=write(client_fd, MSG_STR, strlen(MSG_STR));
				 if(rv < 0)
				 {
					 printf("Write to client by sockfd[%d] failure: %s\n", client_fd, strerror(errno));
					 close(client_fd);
					 exit(0);
				 }
			 } /* Child process loop */
		 } /* Child process start*/
	 }
	 close(listen_fd);
	 return 0;
}                                 
