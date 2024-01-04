/********************************************************************************
 *      Copyright:  (C) 2024 South-Central Minzu University
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2024年01月03日)
 *         Author:  lijinxuan <lijinxuan@gemial.com>
 *      ChangeLog:  1, Release initial version on "2024年01月03日 17时34分22秒"
 *                 
 ********************************************************************************/

#ifndef _SOCKET_H_
#define _SOCKET_H_

typedef struct socketinfo_s
{       
        int   fd;
        char  hostip[64];
        int   port;
        int   connectfd;
}info_t;

int socket_init(info_t *sock_info,char *ip,int port);
int socket_client_connect(info_t *sock_info);
int socket_diag(info_t *sock_info);


#endif
