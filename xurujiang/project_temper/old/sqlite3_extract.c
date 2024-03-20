/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite3_extract.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 21:49:29"
 *                 
 ********************************************************************************/
#include <stdio.h>  
#include <sqlite3.h>  
  
int main() {  
    sqlite3 *db;  
    char *err_msg = 0;  
    int rc;  
  
    // 打开数据库  
    rc = sqlite3_open("memory", &db);  
    if (rc) {  
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));  
        return(0);  
    } else {  
        fprintf(stdout, "成功打开数据库\n");  
    }  
  
    // 准备SQL查询  
    const char *sql = "SELECT * FROM company";  
    sqlite3_stmt *stmt;  
  
    // 编译SQL查询  
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);  
    if (rc != SQLITE_OK) {  
        fprintf(stderr, "SQL错误: %s\n", sqlite3_errmsg(db));  
        return(0);  
    } else {  
        fprintf(stdout, "查询准备成功\n");  
    }  
  
    // 执行查询并提取结果  
    while (sqlite3_step(stmt) == SQLITE_ROW) {  
        int column_count = sqlite3_column_count(stmt);  
        for (int i = 0; i < column_count; i++) {  
            const char *column_name = sqlite3_column_name(stmt, i);  
            const unsigned char *column_data = sqlite3_column_text(stmt, i);  
            printf("%s: %s\n", column_name, column_data ? (const char*)column_data : "NULL");  
        }  
        printf("\n");  
    }  
  
    // 清理并关闭数据库连接  
    sqlite3_finalize(stmt);  
    sqlite3_close(db);  
  
    return 0;  
}

