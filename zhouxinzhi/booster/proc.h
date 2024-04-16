/********************************************************************************
 *      Copyright:  (C) 2020 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  proc.h
 *    Description:  This head file is for Linux process/thread API
 *
 *        Version:  1.0.0(7/06/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "7/06/2012 09:21:33 PM"
 *
 ********************************************************************************/

#ifndef __PROC_H_
#define __PROC_H_

#include <signal.h>
#include <time.h>

#define PID_ASCII_SIZE  11

typedef struct proc_signal_s
{
    int       signal;
    unsigned  stop;     /* 0: Not term  1: Stop  */
}  proc_signal_t;

typedef void *(* thread_body_t) (void *thread_arg);

extern proc_signal_t    g_signal;

/* install default signal process functions  */
extern void install_default_signal(void);

/* excute a linux command by system() */
extern void exec_system_cmd(const char *format, ...);

/* check program already running or not, if not then run it and record pid into $pidfile */
extern int check_set_program_running(int daemon, char *pidfile);

/* check program already running or not from $pid_file  */
extern int check_daemon_running(const char *pid_file);

/* set program daemon running and record pid in $pid_file  */
extern int set_daemon_running(const char *pid_file);

/* record proces ID into $pid_file  */
extern int record_daemon_pid(const char *pid_file);

/* stop program running from $pid_file  */
extern int stop_daemon_running(const char *pid_file);

/* my implementation for set program running in daemon   */
extern void daemonize(int nochdir, int noclose);

/* start a new thread to run $thread_workbody point function  */
extern int thread_start(pthread_t *thread_id, thread_body_t thread_workbody, void *thread_arg);

/* +---------------------+
 * |   Low level API     |
 * +---------------------+*/

/* get daemon process ID from $pid_file   */
extern pid_t get_daemon_pid(const char *pid_file);

/* +------------------------+
 * |  inline functions API  |
 * +------------------------+*/
static inline void msleep(unsigned long ms)
{
    struct timespec cSleep;
    unsigned long ulTmp;

    cSleep.tv_sec = ms / 1000;
    if (cSleep.tv_sec == 0)
    {
        ulTmp = ms * 10000;
        cSleep.tv_nsec = ulTmp * 100;
    }
    else
    {
        cSleep.tv_nsec = 0;
    }

    nanosleep(&cSleep, 0);
    return ;
}

#endif
