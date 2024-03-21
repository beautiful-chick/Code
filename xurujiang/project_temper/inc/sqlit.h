/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlit.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(21/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "21/03/24 19:13:57"
 *                 
 ********************************************************************************/

#ifndef SQLIT_H
#define SQLIT_H
#include "socket_client_option.h"
#include "sqlite3.h"
	void	sqlite3_open_delect(socket_t *sock);
	void	sqlite3_write(char* buf_temper, socket_t *sock);
	int rc;
	sqlite3 *db;
	char *err_msg = 0;
	char buf[200];


#endif
