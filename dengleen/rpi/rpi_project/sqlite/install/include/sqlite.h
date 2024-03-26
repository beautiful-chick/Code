/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(16/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "16/03/24 10:57:18"
 *                 
 ********************************************************************************/
#ifndef _SQLITE_H_
#define _SQLITE_H_



#define SQLITE_PATH "test2.db"
static int callback(void *data, int argc, char **argv, char **azColName);
int open_or_create_sqlite(sqlite3 *db);
int create_statement(sqlite3 *db, char *sql);
int insert_statement(sqlite3 *db, char *sql, char id[],  char date_time[], float *temp);
int select_statement(sqlite3 *db, char *sql);
int update_statement(sqlite3 *db, char *sql);
int delete_statement(sqlite3 *db, char *sql);





#endif

