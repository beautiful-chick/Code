/********************************************************************************
 *      Copyright:  (C) 2020 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  database.h
 *    Description:  This library used to operate blob packet in sqlite database.
 *
 *        Version:  1.0.0(2020年05月13日)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "2020年05月13日 12时14分23秒"
 *
 ********************************************************************************/
#ifndef  _DATABASE_H_
#define  _DATABASE_H_

#include "sqlite3.h"

#define SQL_COMMAND_LEN        256

/*  description: open or create sqlite database if not exist
 *   input args:
 *              $db_file: sqlite database file name
 * return value: <0: failure   0:ok
 * */
extern int database_init(const char *db_file);


/*  description: close sqlite database handler
 * return value: none
 */
extern void database_term(void);


/*  description: push a blob packet into database
 *   input args:
 *               $pack:  blob packet data address
 *               $size:  blob packet data bytes
 * return value: <0: failure   0:ok
 */
extern int database_push_packet(void *pack, int size);


/*  description: pop the first blob packet from database
 *   input args:
 *               $pack:  blob packet output buffer address
 *               $size:  blob packet output buffer size
 *               $byte:  blob packet bytes
 * return value: <0: failure   0:ok
 */
extern int database_pop_packet(void *pack, int size, int *bytes);


/*  description: remove the first blob packet from database
 *   input args: none
 * return value: <0: failure   0:ok
 */
extern int database_del_packet(void);


#endif   /* ----- #ifndef _DATABASE_H_  ----- */
