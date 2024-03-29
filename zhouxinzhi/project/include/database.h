/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  database.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(19/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "19/03/24 12:00:58"
 *                 
 ********************************************************************************/
#ifndef DATA_BASE
#define DATA_BASE
#include <stdio.h>
#include <sqlite3.h>
#include "string_handle.h"
#include "temp.h"

//库操作
sqlite3* sqlite3_open_database(char *db_name);
void sqlite3_close_database(sqlite3 *db);
//表操作
int sqlite3_create_table(sqlite3 *db,char* table_name);
int sqlite3_delect_table(sqlite3 *db,char* table_name);
//数据操作
int sqlite3_insert_single(sqlite3 *db,char *table_name,t_temp_measure *data);
int sqlite3_delect_single(sqlite3 *db,char *table_name);
int sqlite3_query_table(sqlite3 *db,char *table_name);
int callback(void *para,int f_num,char **f_value,char **f_name);
int sqlite3_get_table_data(sqlite3 *db,char *table_name,t_temp_measure *buf);

#endif
