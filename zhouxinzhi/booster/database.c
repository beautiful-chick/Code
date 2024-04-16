/********************************************************************************
 *      Copyright:  (C) 2020 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  database.c
 *    Description:  This library used to operate blob packet in sqlite database.
 *
 *        Version:  1.0.0(2020年05月13日)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "2020年05月13日 12时14分23秒"
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "database.h"
#include "logger.h"

/* Blob packet table name */
#define TABLE_NAME     "PackTable"

/* Use static global handler here in order to simplify the API,
 * But it will make this library not thread safe
 */
static sqlite3         *s_clidb = NULL;


/* description: open or create sqlite database if not exist
 * input args:
 * $db_file: sqlite database file name
 * return value: <0: failure   0:ok
 * */
int database_init(const char *db_file)
{
    char               sql[SQL_COMMAND_LEN]={0};
    char              *errmsg = NULL;

    if( !db_file )
    {
        log_error("%s() Invalid input arguments\n", __func__);
        return -1;
    }

    /*+------------------------------------------+
     *|   database already exist, just open it   |
     *+------------------------------------------+*/
    if( 0==access(db_file, F_OK) )
    {
        if( SQLITE_OK != sqlite3_open(db_file, &s_clidb) )
        {
            log_error("open database file '%s' failure\n", db_file);
            return -2;
        }
        log_info("open database file '%s' ok\n", db_file);
        return 0;
    }

    /*+-----------------------------------------+
     *|  database not exist, create and init it |
     *+-----------------------------------------+*/

    if( SQLITE_OK != sqlite3_open(db_file, &s_clidb) )
    {
        log_error("create database file '%s' failure\n", db_file);
        return -2;
    }

    /* SQLite continues without syncing as soon as it has handed data off to the operating system */
    sqlite3_exec(s_clidb, "pragma synchronous = OFF; ", NULL, NULL, NULL);

    /* enable full auto vacuum, Auto increase/decrease  */
    sqlite3_exec(s_clidb, "pragma auto_vacuum = 2 ; ", NULL, NULL, NULL);

    /* Create firehost table in the database */
    snprintf(sql, sizeof(sql), "CREATE TABLE %s(packet BLOB);", TABLE_NAME);
    if( SQLITE_OK != sqlite3_exec(s_clidb, sql, NULL, NULL, &errmsg) )
    {
        log_error("create data_table in database file '%s' failure: %s\n", db_file, errmsg);
        sqlite3_free(errmsg); /* free errmsg  */
        sqlite3_close(s_clidb);   /* close databse */
        unlink(db_file);      /* remove database file */
        return -3;
    }

    log_info("create and init database file '%s' ok\n", db_file);
    return 0;
}


/* description: close sqlite database handler
 * return value: none
 */
void database_term(void)
{
    log_warn("close sqlite database now\n");
    sqlite3_close(s_clidb);

    return ;
}


/* description: push a blob packet into database
 * input args:
 *      $pack:  blob packet data address
 *      $size:  blob packet data bytes
 * return value: <0: failure   0:ok
 */
int database_push_packet(void *pack, int size)
{
    char               sql[SQL_COMMAND_LEN]={0};
    int                rv = 0;
    sqlite3_stmt      *stat = NULL;

    if( !pack || size<=0 )
    {
        log_error("%s() Invalid input arguments\n", __func__);
        return -1;
    }

    if( ! s_clidb )
    {
        log_error("sqlite database not opened\n");
        return -2;
    }

    snprintf(sql, sizeof(sql), "INSERT INTO %s(packet) VALUES(?)", TABLE_NAME);
    rv = sqlite3_prepare_v2(s_clidb, sql, -1, &stat, NULL);
    if(SQLITE_OK!=rv || !stat)
    {
        log_error("blob add sqlite3_prepare_v2 failure\n");
        rv = -2;
        goto OUT;
    }

    if( SQLITE_OK != sqlite3_bind_blob(stat, 1, pack, size, NULL) )
    {
        log_error("blob add sqlite3_bind_blob failure\n");
        rv = -3;
        goto OUT;
    }

    rv = sqlite3_step(stat);
    if( SQLITE_DONE!=rv && SQLITE_ROW!=rv )
    {
        log_error("blob add sqlite3_step failure\n");
        rv = -4;
        goto OUT;
    }

OUT:
    sqlite3_finalize(stat);

    if( rv < 0 )
        log_error("add new blob packet into database failure, rv=%d\n", rv);
    else
        log_info("add new blob packet into database ok\n");

    return rv;
}


/* description: pop the first blob packet from database
 * input args:
 *      $pack:  blob packet output buffer address
 *      $size:  blob packet output buffer size
 *      $byte:  blob packet bytes
 * return value: <0: failure   0:ok
 */
int database_pop_packet(void *pack, int size, int *bytes)
{
    char               sql[SQL_COMMAND_LEN]={0};
    int                rv = 0;
    sqlite3_stmt      *stat = NULL;
    const void        *blob_ptr;

    if( !pack || size<=0 )
    {
        log_error("%s() Invalid input arguments\n", __func__);
        return -1;
    }

    if( ! s_clidb )
    {
        log_error("sqlite database not opened\n");
        return -2;
    }

    /* Only query the first packet record */
    snprintf(sql, sizeof(sql), "SELECT packet FROM %s WHERE rowid = (SELECT rowid FROM %s LIMIT 1);", TABLE_NAME, TABLE_NAME);
    rv = sqlite3_prepare_v2(s_clidb, sql, -1, &stat, NULL);
    if(SQLITE_OK!=rv || !stat)
    {
        log_error("firehost sqlite3_prepare_v2 failure\n");
        rv = -3;
        goto out;
    }

    rv = sqlite3_step(stat);
    if( SQLITE_DONE!=rv && SQLITE_ROW!=rv )
    {
        log_error("firehost sqlite3_step failure\n");
        rv = -5;
        goto out;
    }

    /* 1rd argument<0> means first segement is packet  */
    blob_ptr = sqlite3_column_blob(stat, 0);
    if( !blob_ptr )
    {
        rv = -6;
        goto out;
    }

    *bytes = sqlite3_column_bytes(stat, 0);

    if( *bytes > size )
    {
        log_error("blob packet bytes[%d] larger than bufsize[%d]\n", *bytes, size);
        *bytes = 0;
        rv = -1;
    }

    memcpy(pack, blob_ptr, *bytes);
    rv = 0;

out:
    sqlite3_finalize(stat);
    return rv;
}


/* description: remove the first blob packet from database
 * input args: none
 * return value: <0: failure   0:ok
 */
int database_del_packet(void)
{
    char               sql[SQL_COMMAND_LEN]={0};
    char              *errmsg = NULL;

    if( ! s_clidb )
    {
        log_error("sqlite database not opened\n");
        return -2;
    }

    /*  remove packet from db */
    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), "DELETE FROM %s WHERE rowid = (SELECT rowid FROM %s LIMIT 1);", TABLE_NAME, TABLE_NAME);
    if( SQLITE_OK != sqlite3_exec(s_clidb, sql, NULL, 0, &errmsg) )
    {
        log_error("delete first blob packet from database failure: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -2;
    }
    log_warn("delete first blob packet from database ok\n");

    /*  Vacuum the database */
    sqlite3_exec(s_clidb, "VACUUM;", NULL, 0, NULL);

    return 0;
}

