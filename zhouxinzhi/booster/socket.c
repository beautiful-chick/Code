/********************************************************************************
 *      Copyright:  (C) 2022 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file is for socket API functions
 *
 *        Version:  1.0.0(18/04/22)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "18/04/22 17:09:59"
 *
 ********************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/resource.h>

#include "socket.h"
#include "logger.h"

/*  description: initial socket context
 *   input args:
 *               $sock:  socket context pointer
 *               $host:  connect server hostname for client mode, unused for server mode
 *               $port:  connect server port for client mode or listen port for server mode
 * return value: <0: failure   0:ok
 */
int socket_init(socket_ctx_t *sock, char *host, int port)
{
    if( !sock || port<=0 )
        return -1;

    memset( sock, 0, sizeof(*sock) );
    sock->fd = -1;
    sock->port = port;
    if( host ) /* server no need it */
    {
        strncpy(sock->host, host, HOSTNAME_LEN);
    }

    return 0;
}

/*  description: close socket
 *   input args:
 *               $sock:  socket context pointer
 * return value: <0: failure   0:ok
 */
int socket_term(socket_ctx_t *sock)
{
    if( !sock )
        return -1;

    if( sock->fd > 0)
    {
        close(sock->fd);
        sock->fd = -1;
    }

    return 0;
}

/*  description: socket server start listen
 *   input args:
 *               $sock:  socket context pointer
 * return value: <0: failure   0:ok
 */
#if 0 /* --TBD-- */
int socket_listen(socket_ctx_t *sock)
{
    int                 rv = 0;
    struct sockaddr_in  addr;
    int                 backlog = 13;

    if( !sock )
        return -1;

    set_socket_rlimit(); /* set max open socket count */
}
#endif

/*  description: socket connect to server in block mode
 *   input args:
 *               $sock:  socket context pointer
 * return value: <0: failure   0:ok
 */
int socket_connect(socket_ctx_t *sock)
{
    int                 rv = 0;
    int                 sockfd = 0;
    char                service[20];
    struct addrinfo     hints, *rp;
    struct addrinfo    *res = NULL;
    struct in_addr      inaddr;
    struct sockaddr_in  addr;
    int                 len = sizeof(addr);

    if( !sock )
        return -1;

    socket_term(sock);

    /*+--------------------------------------------------+
     *| use getaddrinfo() to do domain name translation  |
     *+--------------------------------------------------+*/

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* Only support IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; /* TCP protocol */

    /* If $host is a valid IP address, then don't use name resolution */
    if( inet_aton(sock->host, &inaddr) )
    {
        //log_info("%s is a valid IP address, don't use domain name resolution.\n", sock->host);
        hints.ai_flags |= AI_NUMERICHOST;
    }

    /* Obtain address(es) matching host/port */
    snprintf(service, sizeof(service), "%d", sock->port);
    if( (rv=getaddrinfo(sock->host, service, &hints, &res)) )
    {
        log_error("getaddrinfo() parser [%s:%s] failed: %s\n", sock->host, service, gai_strerror(rv));
        return -3;
    }

    /* getaddrinfo() returns a list of address structures. Try each
       address until we successfully connect or bind */
    for (rp=res; rp!=NULL; rp=rp->ai_next)
    {
#if 0
        char                  ipaddr[INET_ADDRSTRLEN];
        struct sockaddr_in   *sp = (struct sockaddr_in *) rp->ai_addr;

        /* print domain name translation result */
        memset( ipaddr, 0, sizeof(ipaddr) );
        if( inet_ntop(AF_INET, &sp->sin_addr, ipaddr, sizeof(ipaddr)) )
        {
            log_info("domain name resolution [%s->%s]\n", sock->host, ipaddr);
        }
#endif

        /*  Create the socket */
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if( sockfd < 0)
        {
            log_error("socket() create failed: %s\n", strerror(errno));
            rv = -3;
            continue;
        }

        /* connect to server */
        rv = connect(sockfd, rp->ai_addr, len);
        if( 0 == rv )
        {
            sock->fd = sockfd;
            log_info("Connect to server[%s:%d] on fd[%d] successfully!\n", sock->host, sock->port, sockfd);
            break;
        }
        else
        {
            /* socket connect get error, try another IP address */
            close(sockfd);
            continue;
        }
    }

    freeaddrinfo(res);
    return rv;
}

/*  description: send data from the socket
 *   input args:
 *               $sock :  socket context pointer
 *               $data :  socket send data
 *               $bytes:  socket send data bytes
 * return value: <0: failure   0:ok
 */
int socket_send(socket_ctx_t *sock, char *data, int bytes)
{
    int            rv = 0;
    int            i = 0;
    int            left_bytes = bytes;

    if( !sock || !data || bytes<= 0 )
        return -1;

    while( left_bytes > 0 )
    {
        rv=write(sock->fd, &data[i], left_bytes);
        if( rv < 0 )
        {
            log_info("socket[%d] write() failure: %s, close socket now\n", sock->fd, strerror(errno));
            socket_term(sock);
            return -2;
        }
        else if( rv == left_bytes )
        {
            log_info("socket send %d bytes data over\n", bytes);
            return 0;
        }
        else
        {
            /* not send over this time, continue to send left data  */
            i += rv;
            left_bytes -= rv;
            continue;
        }
    }

    return 0;
}

/*  description: receive data from the socket
 *   input args:
 *               $sock :  socket context pointer
 *               $buf  :  socket receive data buffer
 *               $size :  socket receive data buffer size
 *               $timeout: receive data time, <=0 will don't timeout
 * return value: <0: failure   0:ok
 */
int socket_recv(socket_ctx_t *sock, char *buf, int size, int timeout)
{
    int               rv = 0;
    fd_set            rdset;
    int               maxfd;

    if( !sock || !buf || size<= 0 )
        return -1;

    memset(buf, 0, size);

    maxfd = sock->fd;
    FD_ZERO(&rdset);
    FD_SET(sock->fd, &rdset);

    if( timeout <= 0 ) /* no timeout  */
    {
        rv=select(maxfd+1, &rdset, NULL, NULL, NULL);
    }
    else
    {
        struct timeval    tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        rv=select(maxfd+1, &rdset, NULL, NULL, &tv);
    }

    if( rv < 0 )
    {
        log_error("select() on socket[%d] got error: %s\n", sock->fd, strerror(errno));
        return -2;
    }
    else if( rv == 0 )
    {
        log_error("select() on socket[%d] get timeout\n", sock->fd);
        return 0;
    }
    else
    {
        rv = read(sock->fd, buf, size);
        if( rv <= 0 )
        {
            log_error("socket[%d] read() failure or got disconnected: %s, close socket now\n", sock->fd, strerror(errno));
            socket_term(sock);
            return -2;
        }
        else
        {
            log_debug("socket[%d] receive %d bytes data\n", sock->fd, rv);
            return rv;
        }
    }
}



/*+-------------------------------------------------------------------+
 *|                socket utils function                              |
 *+-------------------------------------------------------------------+*/


/*  socket connected or not: <0: failure  0:ok */
int sock_check_connect(int sockfd)
{
    struct tcp_info   info;
    int               len=sizeof(info);

    if( sockfd < 0 )
        return -1;

    getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);

    if( TCP_CLOSE==info.tcpi_state || TCP_CLOSING==info.tcpi_state || TCP_CLOSE_WAIT==info.tcpi_state )
    {
        return -3;
    }

    return -0;
}

/* description: set socket listen port as reusable, fix port already used bug  */
int socket_set_reuseaddr(int sockfd)
{
    int opt = 1;
    int len = sizeof (int);

    if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, len))
    {
        log_error("Set socket[%d] option SO_REUSEADDR failed:%s\n", sockfd, strerror(errno));
        return -1;
    }
    log_debug("Set socket[%d] option SO_REUSEADDR ok\n", sockfd);

    return 0;
}

/* set socket as non-block mode, common socket default work as block mode */
int socket_set_nonblock(int sockfd)
{
    int opts;
    /*
     * fcntl may set:
     *
     * EACCES, EAGAIN: Operation is prohibited by locks held by other
     *          processes. Or, operation is prohibited because the file has
     *          been memory-mapped by another process.
     * EBADF:   fd is not an open file descriptor, or the command was F_SETLK
     *          or F_SETLKW and the file descriptor open mode doesn't match
     *          with the type of lock requested.
     * EDEADLK: It was detected that the specified F_SETLKW command would
     *          cause a deadlock.
     * EFAULT:  lock is outside your accessible address space.
     * EINTR:   For F_SETLKW, the command was interrupted by a signal. For
     *          F_GETLK and F_SETLK, the command was interrupted by a signal
     *          before the lock was checked or acquired. Most likely when
     *          locking a remote file (e.g. locking over NFS), but can
     *          sometimes happen locally.
     * EINVAL:  For F_DUPFD, arg is negative or is greater than the maximum
     *          allowable value. For F_SETSIG, arg is not an allowable signal
     *          number.
     * EMFILE:  For F_DUPFD, the process already has the maximum number of
     *          file descriptors open.
     * ENOLCK:  Too many segment locks open, lock table is full, or a remote
     *          locking protocol failed (e.g. locking over NFS).
     * EPERM:   Attempted to clear the O_APPEND flag on a file that has the
     *          append-only attribute set.
     */
    opts = fcntl(sockfd, F_GETFL);
    if (opts < 0)
    {
        log_warn("fcntl() get socket options failure: %s\n", strerror(errno));
        return -1;
    }

    opts |= O_NONBLOCK;

    if (fcntl(sockfd, F_SETFL, opts) < 0)
    {
        log_warn("fcntl() set socket options failure: %s\n", strerror(errno));
        return -1;
    }

    log_debug("Set socket[%d] none blocking\n", sockfd);
    return opts;
}


/* set socket receive and send buffer size in linux kernel space */
int socket_set_buffer(int sockfd, int rsize, int ssize)
{
    int        opt;
    socklen_t  optlen = sizeof(opt);

    if(sockfd < 0)
        return -1;

    /* Get system default receive buffer size, Linux X86: 85K */
    if (getsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &opt, &optlen))
    {
        log_warn("getsockopt() get receive buffer failure: %s\n", strerror(errno));
        return -2;
    }

    /* Only when current receive buffer size larger than the default one will change it  */
    if(rsize > opt)
    {
        opt = (int) rsize;
        if (setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &opt, optlen))
        {
            log_warn("setsockopt() set receive buffer to %d failure: %s\n", opt, strerror(errno));
            return -2;
        }
    }

    /* Get system default send buffer size, Linux X86: 16K */
    if (getsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &opt, &optlen))
    {
        log_warn("getsockopt() get send buffer failure: %s\n", strerror(errno));
        return -3;
    }

    /* Only when current receive buffer size larger than the default one will change it  */
    if(ssize > opt)
    {
        opt = (int) ssize;
        if (setsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &opt, optlen))
        {
            log_warn("setsockopt() set send buffer to %d failure: %s\n", opt, strerror(errno));
            return -3;
        }
    }

    log_info("Set socket[%d] RCVBUF size:%d  SNDBUF size:%d\n", sockfd, rsize, ssize);
    return 0;
}

/*
 * Enable socket SO_KEEPALIVE, if the connection disconnected, any system call on socket
 * will return immediately and errno will be set to "WSAENOTCONN"
 *
 * keepalive is not program related, but socket related, * so if you have multiple sockets,
 * you can handle keepalive for each of them separately.
 *
 * Reference: http://tldp.org/HOWTO/html_single/TCP-Keepalive-HOWTO/
 */
int socket_set_keepalive(int sockfd, int keepintvl, int keepcnt)
{
    int  opt;

    if(sockfd < 0)
        return -1;

    /* Enable the KEEPALIVE flag */
    opt = 1;
    if (setsockopt (sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, sizeof (opt)))
    {
        log_warn("setsockopt() enable SO_KEEPALIVE failure: %s\n", strerror(errno));
        return -2;
    }

    if(keepintvl || keepcnt)
    {
        /*
         *  The tcp_keepidle parameter specifies the interval between the last data packet sent
         *  (simple ACKs are not considered data) and the first keepalive probe; after the
         *  connection is marked to need keepalive, this counter is not used any further.
         *  ~ >: cat /proc/sys/net/ipv4/tcp_keepalive_time
         *  7200
         */
        opt = 3; /* 3 seconds  */
        if (setsockopt (sockfd, SOL_TCP, TCP_KEEPIDLE, (char *) &opt, sizeof (opt)))
        {
            log_error("setsockopt() set TCP_KEEPIDLE to %d seconds failure: %s\n", opt, strerror(errno));
            return -3;
        }

        if((opt=keepintvl) > 0)
        {
            /*
             * The tcp_keepintvl parameter specifies the interval between subsequential keepalive
             * probes, regardless of what the connection has exchanged in the meantime.
             * ~ >: cat /proc/sys/net/ipv4/tcp_keepalive_intvl
             * 75
             */
            if (setsockopt (sockfd, SOL_TCP, TCP_KEEPINTVL, (char *) &opt, sizeof (opt)))
            {
                log_error("setsockopt() set TCP_KEEPINTVL to %d failure: %s\n", opt, strerror(errno));
                return -4;
            }
        }

        if((opt=keepcnt) > 0)
        {
            /*
             * The TCP_KEEPCNT option specifies the maximum number of unacknowledged probes to
             * send before considering the connection dead and notifying the application layer
             * probes to be sent. The value of TCP_KEEPCNT is an integer value between 1 and n,
             * where n is the value of the systemwide tcp_keepcnt parameter.
             * ~ >: cat /proc/sys/net/ipv4/tcp_keepalive_probes
             * 9
             */
            if (setsockopt (sockfd, SOL_TCP, TCP_KEEPCNT, (char *) &opt, sizeof (opt)))
            {
                log_error("setsockopt() set TCP_KEEPCNT to %d failure: %s\n", opt, strerror(errno));
                return -5;
            }
        }
    }

    log_debug("Set socket[%d] KEEPINTVL:%d  KEEPCNT:%d\n", sockfd, keepintvl, keepcnt);
    return 0;
}


/* Set open file description count to max */
void set_socket_rlimit(void)
{
    struct rlimit limit = {0};

    getrlimit(RLIMIT_NOFILE, &limit );
    limit.rlim_cur  = limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &limit );

    log_info("set socket open fd max count to %d\n", limit.rlim_max);
}

