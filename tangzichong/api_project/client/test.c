/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(18/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "18/03/24 14:51:06"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

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

int main (int argc, char **argv)
{
   sqlite3  *db;
   char     *err=0;
   int      rc;
   char     *sql;

   //打开或创建数据库
   rc=sqlite3_open("test.db",&db);
   if(rc)
   {
    fprintf(stderr,"can't open database:%s\n",sqlite3_errmsg(db));
	exit(0);
   }
   else{
    fprintf(stderr,"opened databasease successfully\n");
   }

   //创建表
   sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

   //执行
   rc=sqlite3_exec(db,sql,callback,0,&err);
   if(rc!=SQLITE_OK)
   {
    fprintf(stderr,"sql error:%s\n",err);
	sqlite3_free(err);
   }
   else{
    fprintf(stdout,"records created successfully\n");
   }
   sqlite3_close(db);
	return 0;
} 


