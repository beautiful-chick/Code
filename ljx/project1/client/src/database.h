/********************************************************************************
 *      Copyright:  (C) 2024 South-Central Minzu University
 *                  All rights reserved.
 *
 *       Filename:  database.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2024年01月03日)
 *         Author:  lijinxuan <lijinxuan@gemial.com>
 *      ChangeLog:  1, Release initial version on "2024年01月03日 17时34分33秒"
 *                 
 ********************************************************************************/


#ifndef _DATABASE_H_
#define _DATABASE_H_

#include"socket.h"

int sql_open(char *filename);
int sql_insert(char buf[]);
int check_Data(const char *filename, char **dataArray, int *rows, int *columns);
void print_update_Data(char **data, int rows, int columns,info_t *sock_info);
int delete_data(const char* filename);


#endif
