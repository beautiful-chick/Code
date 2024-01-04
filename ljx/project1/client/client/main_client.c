/*********************************************************************************
 *      Copyright:  (C) 2024 South-Central Minzu University
 *                  All rights reserved.
 *
 *       Filename:  main_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2024年01月03日)
 *         Author:  lijinxuan <lijinxuan@gemial.com>
 *      ChangeLog:  1, Release initial version on "2024年01月03日 17时49分43秒"
 *                 
 ********************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>
#include <errno.h>
#include"database.h"
#include"get_tempature_time.h"
#include"socket.h"

#define FILENAME "client_data.db"
#define MAX_ROWS 100
#define MAX_COLUMNS  3

int main(int argc,char **argv)
{
    int                 rv;
    int                 i = 1;
    float               temp;
    char                *severip;
    int                 port;
    int                 tim;
    time_t              t_abc;
    char                current_time[64];
    char                temp_buf[64];
    char                buf[512];
    char                sev_buf[128];
    char                sql[256];
    size_t              size;
    info_t              socketinfo;
    struct tm           *tdata;

    if(argc < 4)
    {
        printf("Please import %s [SEVSRIP] [PORT] [TIME]\n",argv[0]);
    }
    severip = argv[1];
    port = atoi(argv[2]);
    tim = atoi(argv[3]);
    rv = sql_open(FILENAME);
    if(rv < 0)
        {
                printf("open database error\n");
                return -1;
        }
    printf("open database successful\n");

    socket_init(&socketinfo,severip,port);
    rv = socket_client_connect(&socketinfo);
    if(rv < 0)
        {
                printf("socket create or connect failure\n");
                return -1;
        }
    printf("connect server successful\n");
    while(1)
    {
    rv = get_tempature(&temp);
    if(rv < 0)
    {
        printf("error : %d\n",rv);
        return -1;
    }
    memset(&temp_buf,0,sizeof(temp_buf));
    sprintf(temp_buf,"|tempature : %f| |time :",temp);

//get time
    time(&t_abc);
    tdata = localtime(&t_abc);
    size = strftime(current_time,64,"%Y年%m月%d日 %H:%M:%S",tdata);

    sprintf(buf,"rpi[%d]%s%s|\n",i,temp_buf,current_time);
    //printf("updata to sever...\n");
    socket_diag(&socketinfo);
    if(socketinfo.connectfd == 0)
    {
        printf("与服务器断开，等待重连\n");
        if(socket_client_connect(&socketinfo) < 0)
        {
                //printf("还是没有连接到服务器端...\n");
                printf("将采集到的数据先存入数据库等待服务器重连\n");
                memset(sql,0,sizeof(sql));
                rv = sql_insert(buf);
                if(rv < 0)
                {
                    continue;
                }
                sleep(tim-1);
                continue;
        }
        else
        {       
                printf("与服务器重新连接,重新上传数据...\n");
                printf("检查数据库中有无数据，如果有先上传数据库中的数据\n");
                int rows, columns;
                char *dataArray[MAX_ROWS][MAX_COLUMNS];
                check_Data(FILENAME, (char **)dataArray, &rows, &columns);
                if(rows == 0)
                {
                        printf("数据库中没有数据...\n");
                }
                else
                {       printf("将数据库数据上传服务器...\n");
                        print_update_Data((char **)dataArray,rows,columns,&socketinfo);
                        printf("数据库数据上传完毕,清空数据库\n");
                        sleep(1);
                        delete_data(FILENAME);
                }
        }
    }
    if(write(socketinfo.fd,buf,sizeof(buf)) < 0)
    {
        printf("Write data to sever failure : %s\n",strerror(errno));
        close(socketinfo.fd);
        return -4;
    }

    memset(&sev_buf,0,sizeof(sev_buf));

    if((rv = read(socketinfo.fd,sev_buf,sizeof(sev_buf))) < 0)
    {
        printf("Read data from sever failre : %s\n",strerror(errno));
        return -5;
    }
    else if(0 == rv)
    {
        printf("client and sever disconnect\n");
    }
    printf("Read %d bytes from sever : %s\n",rv,sev_buf);
    sleep(tim-1);
    }
}



