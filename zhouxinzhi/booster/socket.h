/********************************************************************************
 *      Copyright:  (C) 2022 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This head file is for socket API functions
 *
 *        Version:  1.0.0(18/04/22)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "18/04/22 17:09:59"
 *
 ********************************************************************************/

#ifndef  _SOCKET_H_
#define  _SOCKET_H_

#define HOSTNAME_LEN          64

typedef struct socket_ctx_s
{
    char        host[HOSTNAME_LEN]; /* CLIENT: Connect server hostname; SERVER: Unused */
    int         port;               /* CLIENT: Connect server port;     SERVER: listen port */
    int         fd;                 /* socket descriptor  */
} socket_ctx_t;

/*  description: initial socket context
 *   input args:
 *               $sock:  socket context pointer
 *               $host:  connect server hostname for client mode, unused for server mode
 *               $port:  connect server port for client mode or listen port for server mode
 * return value: <0: failure   0:ok
 */
extern int socket_init(socket_ctx_t *sock, char *host, int port);

/*  description: close socket
 *   input args:
 *               $sock:  socket context pointer
 * return value: <0: failure   0:ok
 */
extern int socket_term(socket_ctx_t *sock);

/*  description: socket server start listen
 *   input args:
 *               $sock:  socket context pointer
 * return value: <0: failure   0:ok
 */
extern int socket_listen(socket_ctx_t *sock);

/*  description: socket client connect to server
 *   input args:
 *               $sock:  socket context pointer
 * return value: <0: failure   0:ok
 */
extern int socket_connect(socket_ctx_t *sock);

/*  description: send data from the socket
 *   input args:
 *               $sock :  socket context pointer
 *               $data :  socket send data
 *               $bytes:  socket send data bytes
 * return value: <0: failure   0:ok
 */
extern int socket_send(socket_ctx_t *sock, char *data, int bytes);

/*  description: receive data from the socket
 *   input args:
 *               $sock :  socket context pointer
 *               $buf  :  socket receive data buffer
 *               $size :  socket receive data buffer size
 *               $timeout: receive data time, <=0 will don't timeout
 * return value: <0: failure   0:ok
 */
#define TIMEOUT_NONE       0
extern int socket_recv(socket_ctx_t *sock, char *buf, int size, int timeout);

/*+-------------------------------------------------------------------+
 *|                socket utils function                              |
 *+-------------------------------------------------------------------+*/


/*  socket connected or not: <0: failure  0:ok */
extern int sock_check_connect(int sockfd);

/* description: set socket listen port as reusable, fix port already used bug  */
extern int socket_set_reuseaddr(int sockfd);

/* set socket as non-block mode, common socket default work as block mode */
extern int socket_set_nonblock(int sockfd);

/* set socket receive and send buffer size in linux kernel space */
extern int socket_set_buffer(int sockfd, int rsize, int ssize);

/* set heartbeat keepalive  */
extern int socket_set_keepalive(int sockfd, int keepintvl, int keepcnt);

/*  Set open file description count to max */
extern void set_socket_rlimit(void);

#endif   /* ----- #ifndef _SOCKET_H_  ----- */

