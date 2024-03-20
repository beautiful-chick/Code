/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  read_test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 10:18:55"
 *                 
 ********************************************************************************/

#include "stdio.h"
#include "sqlite3.h"

#define DATABASE "client_data.db"
#define TABLE	 "my_temperature"
void checkRC(int rc,sqlite3 *db)
{
	if(rc != SQLITE_OK)
	{
		fprintf(stderr,"SQL Error: %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}
}

int callback(void *NotUsed,int argc,char **argv,char **azColName)
{
	for(int i=0; i<argc; i++)
	{

		printf("%s = %s\n",azColName[i],argv[i]?argv[i]:"NULL");
	}
	printf("\n");
	return 0;
}

int main()
{
	sqlite3 *db;
	char *sql;
	int rc;

	rc = sqlite3_open(DATABASE, &db);
	checkRC(rc, db);

	sql = "select * from " TABLE; 

	char *zErrMsg = 0;
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "sql error：%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db);

	return 0;
}
