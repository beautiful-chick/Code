/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite3.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(06/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "06/03/24 21:53:04"
 *                 
 ********************************************************************************/
#include <stdio.h>  
#include <sqlite3.h>  
int sqlite3_temper(char *buffer_temper) 
{
	sqlite3 *db;  
	char *zErrMsg = 0;  
	int rc;  
	char *sql;  
	char *addresses[] = {"California", "Texas", "New York", "Florida"};  
	int num_addresses = sizeof(buffer_temper) / sizeof(buffer_temper[0]);
	// 打开数据库  
	rc = sqlite3_open("data_temper.db", &db);  

	if (rc) {  
		fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));  
		return(0);  
	} else {  
		fprintf(stderr, "数据库已打开1\n");  
	}  

	// 创建表  
	sql = "CREATE TABLE IF NOT EXISTS COMPANY( ID INTEGER PRIMARY KEY AUTOINCREMENT, ADDRESS CHAR(150));"; 

	rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);  

	if (rc != SQLITE_OK) {  
		fprintf(stderr, "SQL 错误: %s\n", zErrMsg);  
		sqlite3_free(zErrMsg);  
	} else {  
		fprintf(stdout, "表已创建\n");  
	}  

	// 插入数据  
	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &zErrMsg);  
	if (rc != SQLITE_OK) {  
		fprintf(stderr, "BEGIN TRANSACTION 错误: %s\n", zErrMsg);  
		sqlite3_free(zErrMsg);  
		return -1;  
	}  
	printf("111111111\n");
	// 构建并执行插入语句  
	int i=1; 
	char insert_sql[1024];  
	snprintf(insert_sql, sizeof(insert_sql),  
			"INSERT INTO COMPANY (ADDRESS) VALUES ('%s');", buffer_temper);  
	rc = sqlite3_exec(db, insert_sql, 0, 0, &zErrMsg);  
	if (rc != SQLITE_OK) {  
		fprintf(stderr, "插入错误: %s\n", zErrMsg);  
		sqlite3_free(zErrMsg);  
		return -1 ;// 发生错误时退出循环  
	}  

	printf("2222222222222\n");

	// 提交或回滚事务  
	if (rc == SQLITE_OK) {  
		rc = sqlite3_exec(db, "COMMIT TRANSACTION;", 0, 0, &zErrMsg);  
		if (rc != SQLITE_OK) {  
			fprintf(stderr, "COMMIT TRANSACTION 错误: %s\n", zErrMsg);  
			sqlite3_free(zErrMsg);  
			sqlite3_exec(db, "ROLLBACK TRANSACTION;", 0, 0, NULL); // 回滚事务  
		}  
	}  


	if (rc != SQLITE_OK) {  
		fprintf(stderr, "SQL 错误: %s\n", zErrMsg);  
		sqlite3_free(zErrMsg);  
	} else {  
		fprintf(stdout, "记录已插入\n");  
	}  

	// 关闭数据库  
	sqlite3_close(db);  

	return 0;  
}

int sqlite3_extract()
{
	sqlite3		 *db;
	char 		 *err_msg = 0;
	int 		 rc;

	rc = sqlite3_open("data_temper.db", &db);
	if(rc)
	{
		fprintf(stderr, "无法打开数据库： %s\n", sqlite3_errmsg(db));


	}

	else
	{
		fprintf(stdout, "成功打开数据库1\n");
	}

	const char *sql = "select * from company";
	sqlite3_stmt *stmt;

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL错误1:%s\n", sqlite3_errmsg(db));
		return 0;
	}
	else 
	{
		fprintf(stdout, "查询准备成功");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int column_count = sqlite3_column_count(stmt);
		for(int i = 0; i < column_count; i++ )
		{
			const char *column_name = sqlite3_column_name(stmt, i);
			const unsigned char *column_data = sqlite3_column_text(stmt, i);
			printf("%s: %s\n", column_name, column_data?(const char*)column_data : "NULL");
		}

		printf("\n");

	}


	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return 1;
}
