/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(18/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "18/03/24 15:43:04"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "sqliteall.h"
#include <unistd.h>

data_t da[MAX];
int j=0;

int client_select_statement(sqlite3 *db, char *sql)
{
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open(SQLITE_PATH, &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}

	sql = "SELECT * from COMPANY";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
	}
	sqlite3_close(db);
	return 0;
}


static int callback(void *data, int argc, char **argv, char **azColName)
{
	

	memset(da[j].array, 0, sizeof(da[j].array));
	snprintf(da[j].array, sizeof(da[j].array), "%s %s %s", argv[0],argv[1],argv[2]);
	j++;

	return 0;
}



int delete_statement(sqlite3 *db, char *sql)
{

	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open(SQLITE_PATH, &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}


	sql = "DELETE FROM COMPANY;""SELECT * FROM COMPANY";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Operation done successfully\n");
	}
	sqlite3_close(db);
	j=0;
	return 0;
}


int insert_statement(sqlite3 *db, char *sql, char id[],char date_time[],float *temp)
{

	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open(SQLITE_PATH, &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}


	sql =sqlite3_mprintf("INSERT INTO COMPANY VALUES('%s', '%s', '%f');",id, date_time, *temp);


	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Records created successfully\n");
	}
	sqlite3_close(db);
	return 0;
}


int create_statement(sqlite3 *db, char *sql)
{
	int     rc;
	char   *zErrMsg = 0;

	
	if(access(SQLITE_PATH,F_OK)==0)
		return 0;

	rc = sqlite3_open(SQLITE_PATH, &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}else
	{
		fprintf(stdout, "Opened database successfully\n");
	}


	sql = "CREATE TABLE COMPANY("  \
		   "ID             TEXT                      NOT NULL," \
		   "TADETIME       TEXT   PRIMARY KEY        NOT NULL," \
		   "TEMPERATURE    REAL                      NOT NULL);";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);
	return 0;
}

