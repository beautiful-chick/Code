/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqliteall.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(18/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "18/03/24 16:17:06"
 *                 
 ********************************************************************************/
#ifndef _SQLITE_H_
#define _SQLITE_H_

#define SQLITE_PATH "test2.db"
#define MAX          100

typedef struct data_s
{
	    char    array[64];
}data_t;

extern data_t da[MAX];

int create_statement(sqlite3 *db, char *sql);
int insert_statement(sqlite3 *db, char *sql, char id[],char date_time[],float *temp);
int delete_statement(sqlite3 *db, char *sql);
int client_select_statement(sqlite3 *db, char *sql);
static int callback(void *data, int argc, char **argv, char **azColName);

#endif
