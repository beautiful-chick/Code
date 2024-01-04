/*********************************************************************************
 *      Copyright:  (C) 2024 South-Central Minzu University
 *                  All rights reserved.
 *
 *       Filename:  database.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2024年01月03日)
 *         Author:  lijinxuan <lijinxuan@gemial.com>
 *      ChangeLog:  1, Release initial version on "2024年01月03日 17时26分58秒"
 *                 
 ********************************************************************************/

#include<stdio.h>
#include<string.h>
#include<sqlite3.h>
#include<stdlib.h>
#include<errno.h>
#include <unistd.h>
#include"socket.h"

static sqlite3  *c_db = NULL;
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int sql_open(char *filename)
{
        char                    sql[256];
        int                     rc;
        char                    *errmsg = NULL;
        if( !filename )
        {
                printf("filename is NULL\n");
                sqlite3_free(errmsg);
                return -1;
        }

        if(SQLITE_OK != sqlite3_open(filename,&c_db))
        {
                printf("Error open datadase: %s\n",errmsg);
                sqlite3_free(errmsg);
                return -2;
        }

        memset(sql,0,sizeof(sql));
        snprintf(sql,sizeof(sql),"CREATE TABLE database(id INTEGER PRIMARY KEY,tempature varchar(20) default '0');");

        printf("%s\n",sql);
        rc = sqlite3_exec(c_db,sql,callback,0,&errmsg);
        if(rc != SQLITE_OK )
        {
                fprintf(stderr,"SQL error: %s\n",errmsg);
                sqlite3_free(errmsg);
        }
        else
        {
                fprintf(stdout,"Table created successfully\n");
        }
        return 0;
}

int sql_insert(char buf[])
{
        char            *errmsg = NULL;
        char            sql[128];
        int             rc;

        memset(sql,0,sizeof(sql));
        snprintf(sql,sizeof(sql),"insert into database (tempature) values('%s')",buf);

        rc = sqlite3_exec(c_db,sql,callback,0,&errmsg);
        if(rc != SQLITE_OK)
        {
                printf("sqlite insert table error: %s\n",errmsg);
                sqlite3_free(errmsg);
                return -1;
        }
        printf("insert data successfully\n");
        return 0;
}

int check_Data(const char *filename, char **dataArray, int *rows, int *columns)
{
        sqlite3      *db;
        char         *errMsg = 0;
        int          rc;
        char         **result;
        char         sql[128];

        rc = sqlite3_open(filename, &db);

        if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                return -1;
        }
        memset(sql,0,sizeof(sql));
        snprintf(sql,sizeof(sql),"SELECT tempature FROM database");
        rc = sqlite3_get_table(db, sql, &result, rows, columns, &errMsg);

        if( rc != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", errMsg);
                sqlite3_free(errMsg);
                sqlite3_close(db);
                return -1;
        }

        if (*rows == 0) {
                printf("No data found in database.\n");
                sqlite3_free_table(result);
                sqlite3_close(db);
                return -1;
        }


        for(int i = 0; i < *rows; i++) {
                for(int j = 0; j < *columns; j++) {
                        dataArray[i * *columns + j] = result[(i+1) * *columns + j];
                }
        }

        sqlite3_free_table(result);
        sqlite3_close(db);

        return 0;
}

void print_update_Data(char **data, int rows, int columns,info_t *sock_info) 
{
        int i, j;
        char buf[512];
        for(i = 0; i < rows; i++) 
        {
                printf("数据库第%d行的数据\n",i+1);
                for(j = 0; j < columns; j++) 
                {
                        memset(buf,0,sizeof(buf));
                        printf("%s", data[(i)*columns + j]);
                        sprintf(buf,"%s",data[(i)*columns + j]);
                        write(sock_info->fd,buf,sizeof(buf));
                }
                printf("\n");
                sleep(1);
        }
}


int delete_data(const char* filename) 
{
        sqlite3   *db;
        char      *err_msg = 0;
        char      sql[128];
        int rc = sqlite3_open(filename, &db);
        if (rc != SQLITE_OK)
        {
                fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return 1;
        }
        memset(sql,0,sizeof(sql));
        snprintf(sql,sizeof(sql),"DELETE FROM database;");
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) 
        {
                fprintf(stderr, "无法执行SQL语句: %s\n", err_msg);
                sqlite3_free(err_msg);
                sqlite3_close(db);
                return 1;
        }
        sqlite3_close(db);
        return 0;
}
