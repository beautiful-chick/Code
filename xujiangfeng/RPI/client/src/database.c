#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sqlite3.h>
#include <libgen.h>

#include "main.h"
#include "sqlite3.h"
#include "database.h"
#include "ds18b20.h"

#define TABLE_NAME       "RPI_num1"

static int  count=0;
int         rc=-1;
sqlite3     *db;
char        *zErrMsg=0;
char        write_buffer2[100];
int         iTableExist=0;


static int sendata2_callback(void *data,int argc,char **argv, char **azColName)
{
	memset(write_buffer2,0,sizeof(write_buffer2));
	snprintf(write_buffer2,100,"%s/%s/%s",argv[1],argv[2],argv[3]);
	printf("write_buffer2:%s\n",write_buffer2);
	return 0;
}



int sendata2(int sockfd,char *table_name,int count)
{
	int  write_rv2 = -1;
	char sendata_buffer2[512];
	memset(sendata_buffer2,0,sizeof(sendata_buffer2));
	snprintf(sendata_buffer2,512,"select * from %s limit 1 offset %d;",table_name,(count-1));

	rc=sqlite3_exec(db,sendata_buffer2,sendata2_callback,0,&zErrMsg);

	if(rc != SQLITE_OK)
	{
		printf("sendata_buffer2 failure:%d %s\n",rc,zErrMsg);
		sqlite3_free(zErrMsg);
		exit(0);
	}

	write_rv2=write(sockfd,write_buffer2,strlen(write_buffer2));
	if(write_rv2<0)
	{
		printf("Send data to server failure:%s\n",strerror(errno));
		return -1;
	}
	printf("Send data2 to server successfully!\n");
	return 0;
}


int sendata(int sockfd,char *buftok[])
{
	int  write_rv1 = -1;
	char sendata_buffer1[100];
	memset(sendata_buffer1,0,sizeof(sendata_buffer1));
	snprintf(sendata_buffer1,100,"%s/%s/%s",buftok[1],buftok[2],buftok[3]);

	write_rv1=write(sockfd,sendata_buffer1,strlen(sendata_buffer1));
	if(write_rv1<0)
	{
		printf("Send data to server failure:%s\n",strerror(errno));
		return -1;
	}
	printf("Send data %s to server successfully!\n",sendata_buffer1);
	return 0;
}




int sqlite_table_exist(char *table_name)
{
	char table_exist[512];
	memset(table_exist,0,sizeof(table_exist));
	snprintf(table_exist,512,"SELECT COUNT(*) from sqlite_master where type='table' and name ='%s';",table_name);

	rc=sqlite3_exec(db,table_exist,table_exist_callback,0,&zErrMsg);
	if(rc != SQLITE_OK)
	{
		printf("sqlite_table_exist error:%s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		exit(0);
	}

	if(iTableExist)
	{
		printf("%s is exist!\n",table_name);
		return iTableExist;
	}
	else
	{
		return iTableExist;
	}
}



static int table_exist_callback(void *data,int argc,char **argv,char **azColName)
{
	if(1==argc)
	{
		iTableExist=atoi(*(argv));
	}
	return 0;
}


static int callback_row_count(void *NotUsed, int argc,char **argv,char **azColName)
{
	count=atoi(argv[0]);
	return 0;
}



int table_row_count(char *table_name)
{
	char row_count[100];
	memset(row_count,0,sizeof(row_count));
	snprintf(row_count,128,"SELECT COUNT(*) from %s;",table_name);

	rc=sqlite3_exec(db,row_count,callback_row_count,0,&zErrMsg);
	if(rc != SQLITE_OK)
	{
		printf("table_row_count error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		return count;
	}
}



int sqlite_delete(int id,char *table_name)
{
	char sql_delete[100];
	memset(sql_delete,0,sizeof(sql_delete));
	snprintf(sql_delete,100,"DELETE from %s where ID=%d;SELECT * from %s;",table_name,count,table_name);

	rc=sqlite3_exec(db,sql_delete,0,0,&zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr,"Sqlite_delete error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	else
	{
		return 0;
	}
}



void sqlite_insert(char *tokbuf[],char *table_name)
{
	char sql_buffer[512];
	char *ID=tokbuf[0];
	char *SN=tokbuf[1];
	char *DATIME=tokbuf[2];
	char *TEMPERATURE=tokbuf[3];


	memset(sql_buffer,0,sizeof(sql_buffer));
	printf("Insert data to table %s:%s/%s/%s/%s\n",TABLE_NAME,tokbuf[0],tokbuf[1],tokbuf[2],tokbuf[3]);

	snprintf(sql_buffer,512,"INSERT INTO %s VALUES ('%s','%s','%s','%s');",table_name,ID,SN,DATIME,TEMPERATURE);

	rc=sqlite3_exec(db,sql_buffer,callback,0,&zErrMsg);
	if(rc != SQLITE_OK)
	{
		printf("Sqlite_insert fuilure:%s\n",zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Last data insert table successfully\n");
	}
	return;
}



void sqlite_create_table(char *table_name)
{
	char create_table[512];

	memset(create_table,0,sizeof(create_table));
	snprintf(create_table,512,"CREATE TABLE %s(ID CHAR(10),SN CHAR(10),DATIME CHAR(50),TEMPERATURE CHAR(50));",table_name);

	rc=sqlite3_exec(db,create_table,callback,0,&zErrMsg);
	if(rc != SQLITE_OK)
	{
		printf("Sqlite_create_table fuilure:%s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		exit(0);
	}
	else
	{
		printf("Table created successfully!\n");
	}
	return;
}




static int callback(void *NotUsed,int argc,char **argv,char **azColName)
{
	int i;
	for(i=0;i<argc;i++)
	{
		printf("%s = %s\n",azColName[i],argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}



void sqlite_init(void)
{
	rc=sqlite3_open("RPI_temperature1.db",&db);
	if(rc)
	{
		printf("Can't open database:%s\n",sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		printf("Opened database successfully!\n");
	}
}

