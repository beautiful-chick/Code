/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(13/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "13/03/24 23:51:30"
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
#include <sqlite3.h>
#include <signal.h>
#include "ds18b20.h"


static inline void print_usage( char *progname)
{
        printf("usage:%s\n",progname);
        printf("-i( -- ipaddr): specify server Ip address\n");
        printf("-p(--port): specify server PORT\n");
        printf("-h(--help): print this help information\n");
        printf("-t(--time): print this time information\n");
        return;
}

static int callback(void *notused,int argc,char **argv,char **azcolname)
{
   int i;
   for(i=0;i<argc;i++)
   {
    printf("%s=%s\n",azcolname[i],argv[i] ? argv[i]:"NULL");

   }
   printf("\n");
   return 0;
}

int check_socket(int sock)
{
   int err=0;
   socklen_t len =sizeof(err);
   int retval;
   retval=getsockopt(sock,SOL_SOCKET,SO_ERROR,&err,&len);
   if(!err)
   {
     perror("socket error");
   }

}

int main (int argc, char **argv)
{
   int                  client_fd = -1;
   struct sockaddr_in   serv_addr;
   int                  rv=0;
   char                 buf[1024];
   char                 t_buff[128];
   int                  opt;
   char                 *optaddress=NULL;
   int                  port=0;
   int                  opttime=0;
   int                  con=-1;
   int                  rc;
   float                temp;
   char                 dev[1024];
   char                 print_buf[1024];
   sqlite3              *db;
   char                 *sql;
   char                 *errmsg;
   int                  sock;
   struct option        opts[] ={
        {"ip", required_argument, NULL, 'i'},
        {"port", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
		{"time",required_argument,NULL,'h'},
        {NULL, 0, NULL, 0}
        };

   while((opt=getopt_long(argc,argv,"i:p:h:t:",opts,NULL))!=-1)
   {
       switch(opt)
	   {
	          case 'i':
		               optaddress=optarg;
					   break;
			  case 'p':
					   port=atoi(optarg);
					   break;
              case 'h':
					   print_usage(argv[0]);
					   break;
			  case 't':
                      opttime=atoi(optarg);
					  break;
       }
   }

#if 0
   if(!optaddress || !port || !opttime)
   {
    print_usage(argv[0]);
	return -2;
   }
#endif

   client_fd=socket(AF_INET, SOCK_STREAM, 0);
   if(client_fd<0)
   {
    printf("create socket failure:%s\n",strerror(errno));
	return -1;
   }
   printf("create socket successful\n");

   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(port);
   inet_aton(optaddress, &serv_addr.sin_addr);
   
   con=connect(client_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
   if(con<0)
   {
       printf("connect to server[%s:%d] failure:%s\n",optaddress,port,strerror(errno));
	   
   }
   printf("connected successful\n");
   
  signal(SIGPIPE,SIG_IGN);
 while(1)
 {
   if(con<0)
   {
      memset(t_buff,0,sizeof(t_buff));
      get_time(t_buff);
      get_temp(&temp);
     if(get_temp(&temp) < 0)
     {
       printf("ERROR:ds18b20 get temperature failure\n");
       goto cleanup;
    
     } 
     get_devid(dev,sizeof(dev));
     memset(print_buf,0,sizeof(print_buf));
     snprintf(print_buf,sizeof(print_buf),"\ndevid:%s\n time:%s\n temperature:%f\n",dev,t_buff,temp);
	 printf("%s\n",print_buf);

	 if(sqlite3_open("temp.db",&db)!=SQLITE_OK)
	 {
	   fprintf(stderr,"cannot open dataabase:%s\n",sqlite3_errmsg(db));
	   sqlite3_close(db);
	 }
	 else{
	   fprintf(stdout,"create database successfully!\n");
	 }

	 sql="CREATE TABLE IF NOT EXISTS Samples(data NOT NULL)";
	 rc=sqlite3_exec(db,sql,callback,0,&errmsg);
	 if(rc!=SQLITE_OK)
	 {
       fprintf(stderr,"sql error:%s\n",errmsg);
	   sqlite3_free(errmsg);
	 }
	 else{
       fprintf(stdout,"create Samples successfully\n");
	 }

	 sql=sqlite3_mprintf("INSERT INTO Samples(data)" "VALUES('%s');",print_buf);
     rc=sqlite3_exec(db,sql,callback,0,&errmsg);
     if(rc)
	 {
      	fprintf(stderr,"insert error: %s\n",sqlite3_errmsg(db));

	 }
	 else{
      fprintf(stdout,"insert successfully\n");
	 }

	 if(check_socket(sock)<0)
	 {

    
   
	 }

   else
   {
   memset(t_buff,0,sizeof(t_buff));
   get_time(t_buff);

   get_temp(&temp);

    if(get_temp(&temp) < 0)
    {
       printf("ERROR:ds18b20 get temperature failure\n");
       goto cleanup;
    
    }

   get_devid(dev,sizeof(dev));
   
   memset(print_buf,0,sizeof(print_buf));
   snprintf(print_buf,sizeof(print_buf),"\ndevid:%s\n time:%s\n temperature:%f\n",dev,t_buff,temp);
   printf("%s",print_buf);
   if(write(client_fd,print_buf,strlen(print_buf))<0)
   {
      printf("write to data to [%d:%s] failure:%s\n",port,optaddress,strerror(errno));
     // goto cleanup;
	 con=-1;
   }
   sleep(2);
   }
#if 0
   memset(buf,0,sizeof(buf));
   rv=read(client_fd,buf,sizeof(buf));
	if(rv<0)
	{
     printf("read data to [%s:%d] failure:%s\n",optaddress,port,strerror(errno));
     //goto cleanup;
	}

   else if(rv==0)
   {
    printf("client disconnected to server\n");
	//goto cleanup;
   }
   printf("read %d byte data from server:%s\n",rv,buf);
  
    sleep(2);
#endif
 }
cleanup:
   close(client_fd);
	
} 
