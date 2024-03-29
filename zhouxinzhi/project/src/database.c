/****
 * ***************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  database.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(15/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "15/03/24 18:55:21"
 *                 
 ********************************************************************************/
#include "database.h"

//打开数据库
sqlite3* sqlite3_open_database(char *db_name)
{	
	sqlite3						*db = NULL;
	int 						rv = -1;
	
	if ( (db_name == NULL) || (strlen(db_name) == 0) )
	{
		return NULL;
	}
	
	if ( (rv = sqlite3_open(db_name,&db)) != SQLITE_OK )
	{
		printf("open db failture:%s\n",strerror(errno));
		return NULL;
	}
	
	return db;
}

//关闭数据库
void sqlite3_close_database(sqlite3 *db)
{
	if ( db == NULL)
	{
		printf("close db failture%s\n",strerror(errno));
	}
	sqlite3_close(db);
}

//若表不存在，则创建表
int sqlite3_create_table(sqlite3 *db,char* table_name)
{
	int							rv = -1;
	char						sql_str[128] = {0}; 						
	char 						*err_msg = 	NULL;

	sprintf(sql_str,"CREATE TABLE IF NOT EXISTS %s(dev_name text,meas_time text,temp real);",table_name);

	if ( (rv = sqlite3_exec(db,sql_str,0,0,&err_msg)) != SQLITE_OK)
	{
		printf("creat table:%s error:%s\n",table_name,err_msg);
		sqlite3_free(err_msg);
		return -1;
	}
	return 0;
}

//删除一个表
int sqlite3_delect_table(sqlite3 *db,char* table_name)
{
	int							rv = -1;
	char						sql_str[128] = {0};
    char						*err_msg = NULL;

	sprintf(sql_str,"DROP TABLE %s",table_name);

	if ( (rv = sqlite3_exec(db,sql_str,0,0,&err_msg)) != SQLITE_OK )
	{
		printf("delete table:%s failture:%s\n",table_name,err_msg);
		sqlite3_free(err_msg);
		return -1;
	}

	return 0;
}

//添加一行特定的数据
int sqlite3_insert_single(sqlite3 *db,char *table_name,t_temp_measure *data)
{
	int							rv = -1;
	char 						sql_str[128] = {0};
	char						*err_msg = NULL;
	char						buf[128];

	//getdate_time(data->meas_time,buf);

	sprintf(sql_str,"INSERT INTO %s(dev_name,meas_time,temp) VALUES ('%s','%s',%f);"
				   ,table_name,data->dev_name,data->ptime,data->temp);

	if ( (rv = sqlite3_exec(db,sql_str,0,0,&err_msg)) != SQLITE_OK )
	{
			printf("insert database error:%s",err_msg);
			sqlite3_free(err_msg);
			return -1;
	}

	return 0;
}

//删除一行数据
int sqlite3_delect_single(sqlite3 *db,char *table_name)
{
	int 						rv = -1;
	char 						sql_str[128] = {0};
	char						*err_msg = NULL;
	
	sprintf(sql_str,"DELETE FROM %s WHERE ROWID IN (SELECT ROWID FROM %s LIMIT 1);",table_name,table_name);

	if ( (rv = sqlite3_exec(db,sql_str,0,0,&err_msg)) != SQLITE_OK )
	{
		printf("delete data error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}

	return 0;
}

//查询数据(打印整张表)
int sqlite3_query_table(sqlite3 *db,char *table_name)
{
	int 						rv = -1;
	char 						sql_str[128] = {0};
	char  						*err_msg = NULL;
	
	printf("dev_name\t measure_time\t\t temperature\n");
	sprintf(sql_str,"SELECT * FROM %s;",table_name);

	if ( (rv = sqlite3_exec(db,sql_str,callback,NULL,&err_msg)) != SQLITE_OK )
	{
		printf("select table error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -2;
	}

	return 0;
}
//select的回调函数
int callback(void *para,int f_num,char **f_value,char **f_name)
{
	int 						 i = 0;
 
	for (i=0; i<f_num; i++)
	{
		printf("%s\t",f_value[i]);
	}
	printf("\n");

	return 0;
}

//查询数据库的第一条数据是否为空,若非空将返回数据
int sqlite3_get_table_data(sqlite3 *db,char *table_name,t_temp_measure *buf)
{
	int							 rv = -1;
	int							 row = -1;
	int							 colnm = -1;
	char 						 sql_str[128] = {0};
	char 						 *err_msg = NULL;
	char 						 **dresult = NULL;

	sprintf(sql_str,"SELECT * FROM %s;",table_name);

	rv = sqlite3_get_table(db,sql_str,&dresult,&row,&colnm,&err_msg);
	
	if ( (rv == SQLITE_OK) && (row > 0) )
	{
		strncpy(buf->dev_name,dresult[3],128);
		buf->temp = atof(dresult[5]);
		strncpy(buf->ptime,dresult[4],128);
		return 1;
	}
	else
	{
		return 0;
	}
	
}
