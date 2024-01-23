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
#include <sqlite3.h>

#include "main.h"
#include "database.h"
#include "sqlite3.h"

#define TABLE_NAME "RPI_num2"

sqlite3     *db;
char        *zErrMsg;
int         rc;
int         iTableExist=0;
int         clifd;

void sqlite_insert(char *buftok[], char *table_name)
{
	char   sql_buffer[512];
	char   *SN1 = buftok[0];
	char   *DATIME1 = buftok[1];
	char   *TEMPERATURE1 = buftok[2];

	memset(sql_buffer,0,sizeof(sql_buffer));
	printf("Accept client[%d] data successfully!\n", clifd);

	snprintf(sql_buffer,512, "INSERT INTO %s VALUES ('%s', '%s', '%s');" ,table_name, SN1, DATIME1, TEMPERATURE1);

	rc = sqlite3_exec(db, sql_buffer, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		printf("sqlite_insert: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Records created successfully\n");
	}
	return;
}



int sqlite_table_exist(char *table_name)
{
	char   table_exist[512];

	memset(table_exist,0,sizeof(table_exist));
	snprintf(table_exist,512,"SELECT COUNT(*) FROM sqlite_master where type ='table' and name ='%s';", table_name);

	rc = sqlite3_exec(db, table_exist, table_exist_callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		printf("sqlite_table_exist:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		exit(0);
	}

	if(iTableExist)
	{
		printf("%s is exist!\n", table_name);
		return iTableExist;
	}
	else
	{
		return iTableExist;
	}
}



static int table_exist_callback(void *data, int argc, char **argv, char **azColName)
{
	if ( 1 == argc)
	{
		iTableExist = atoi(*(argv));
	}
	return 0;
}



void sqlite_create_table(char *table_name)
{
	char create_table[100];
	memset(create_table,0,sizeof(create_table));
	snprintf(create_table,100,"CREATE TABLE %s(SN CHAR(10),DATIME CHAR(50),TEMPERATURE CHAR(10));", table_name);

	rc = sqlite3_exec(db, create_table, callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		printf("sqlite_create_table:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		exit(0);
	}
	else
	{
		printf("Table created successfully!\n");
	}
	return;
}



static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}



void  sqlite_init(void)
{
	rc = sqlite3_open("RPI_temperature2.db", &db);
	if(rc)
	{
		printf("Can't open database:%s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		printf("Opened database successfully\n");
	}
}



