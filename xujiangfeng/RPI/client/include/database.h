
#ifndef _DATABASE_H_
#define _DATABASE_H_

static int sendata2_callback(void *data,int argc,char **argv, char **azColName);
int sendata2(int sockfd,char *table_name,int count);
int sendata(int sockfd,char *buftok[]);
int sqlite_table_exist(char *table_name);
static int table_exist_callback(void *data,int argc,char **argv,char **azColName);
static int callback_row_count(void *NotUsed, int argc,char **argv,char **azColName);
int table_row_count(char *table_name);
int sqlite_delete(int id,char *table_name);
void sqlite_insert(char *buftok[],char *table_name);
void sqlite_create_table(char *table_name);
void sqlite_init(void);
static int callback(void *NotUsed,int argc,char **argv,char **azColName);

#endif
