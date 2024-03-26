/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  client.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(26/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "26/03/24 17:08:49"
 *                 
 ********************************************************************************/
#ifndef _CLIENT_H_
#define _CLIENT_H_



#define CONFIG_DEBUG
#ifdef CONFIG_DEBUG
#define dbg_print(format, args...) printf(format, ##args)
#else
#define dbg_print(format, args...) do{} while(0)
#endif

int connect_server(char *serv_ip, int serv_port, int *conn_fd);

#endif
