
#ifndef _DATABASE_H_
#define _DATABASE_H_

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
void  sqlite_init(void);
void sqlite_create_table(char *table_name);
void sqlite_insert(char *buftok[], char *table_name);
int sqlite_table_exist(char *table_name);
static int table_exist_callback(void *data, int argc, char **argv, char **azColName);

#endif
