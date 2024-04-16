/*********************************************************************************
 *      Copyright:  (C) 2020 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  proc.c
 *    Description:  This file is the process API
 *
 *        Version:  1.0.0(7/06/2020)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "7/06/2020 09:19:02 PM"
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "proc.h"
#include "logger.h"

proc_signal_t     g_signal={0};

void proc_default_sighandler(int sig)
{
    switch(sig)
    {
        case SIGINT:
            log_warn("SIGINT - stopping\n");
            g_signal.stop = 1;
            break;

        case SIGTERM:
            log_warn("SIGTERM - stopping\n");
            g_signal.stop = 1;
            break;

        case SIGSEGV:
            log_warn("SIGSEGV - stopping\n");
#if 0
            if(g_signal.stop)
                exit(0);

            g_signal.stop = 1;
#endif
            break;

        case SIGPIPE:
            log_warn("SIGPIPE - warnning\n");
            break;

        default:
            break;
    }
}


/* install default signal process functions  */
void install_default_signal(void)
{
    struct sigaction sigact, sigign;

    log_info("Install default signal handler.\n");

    /*  Initialize the catch signal structure. */
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = proc_default_sighandler;

    /*  Setup the ignore signal. */
    sigemptyset(&sigign.sa_mask);
    sigign.sa_flags = 0;
    sigign.sa_handler = SIG_IGN;

    sigaction(SIGTERM, &sigact, 0); /*  catch terminate signal "kill" command */
    sigaction(SIGINT,  &sigact, 0); /*  catch interrupt signal CTRL+C */
    //sigaction(SIGSEGV, &sigact, 0); /*  catch segmentation faults  */
    sigaction(SIGPIPE, &sigact, 0); /*  catch broken pipe */
#if 0
    sigaction(SIGCHLD, &sigact, 0); /*  catch child process return */
    sigaction(SIGUSR2, &sigact, 0); /*  catch USER signal */
#endif
}


/* ****************************************************************************
 * FunctionName: daemonize
 * Description : Set the programe runs as daemon in background
 * Inputs      : nodir: DON'T change the work directory to / :  1:NoChange 0:Change
 *               noclose: close the opened file descrtipion or not 1:Noclose 0:Close
 * Output      : NONE
 * Return      : NONE
 * *****************************************************************************/
void daemonize(int nochdir, int noclose)
{
    int rv, fd;
    int i;

    /*  already a daemon */
    if (1 == getppid())
        return;

    /*  fork error */
    rv = fork();
    if (rv < 0) exit(1);

    /*  parent process exit */
    if (rv > 0)
        exit(0);

    /*  obtain a new process session group */
    setsid();

    if (!noclose)
    {
        /*  close all descriptors */
        for (i = getdtablesize(); i >= 0; --i)
        {
            //if (i != g_logPtr->fd)
                close(i);
        }

        /*  Redirect Standard input [0] to /dev/null */
        fd = open("/dev/null", O_RDWR);

        /* Redirect Standard output [1] to /dev/null */
        dup(fd);

        /* Redirect Standard error [2] to /dev/null */
        dup(fd);
    }

    umask(0);

    if (!nochdir)
        chdir("/");

    return;
}

/* ****************************************************************************
 * FunctionName: check_set_program_running
 * Description : check program already running or not, if not then run it and
 *               record pid into $pidfile
 * Inputs      : daemon:  set program running in daemon or not
 *               pid_file:The record PID file path
 * Output      : NONE
 * Return      : 0: Record successfully  Else: Failure
 * *****************************************************************************/

int check_set_program_running(int daemon, char *pidfile)
{
    if( !pidfile )
        return 0;

    if( check_daemon_running(pidfile) )
    {
        log_error("Program already running, process exit now\n");
        return -1;
    }

    if( daemon )
    {
        if( set_daemon_running(pidfile) < 0 )
        {
            log_error("set program running as daemon failure\n");
            return -2;
        }
    }
    else
    {
        if( record_daemon_pid(pidfile) < 0 )
        {
            log_error("record program running PID failure\n");
            return -3;
        }
    }

    return 0;
}



/* ****************************************************************************
 * FunctionName: record_daemon_pid
 * Description : Record the running daemon program PID to the file "pid_file"
 * Inputs      : pid_file:The record PID file path
 * Output      : NONE
 * Return      : 0: Record successfully  Else: Failure
 * *****************************************************************************/
int record_daemon_pid(const char *pid_file)
{
    struct stat fStatBuf;
    int fd = -1;
    int mode = S_IROTH | S_IXOTH | S_IRGRP | S_IXGRP | S_IRWXU;
    char ipc_dir[64] = { 0 };

    strncpy(ipc_dir, pid_file, 64);

    /* dirname() will modify ipc_dir and save the result */
    dirname(ipc_dir);

    /* If folder pid_file PATH doesnot exist, then we will create it" */
    if (stat(ipc_dir, &fStatBuf) < 0)
    {
        if (mkdir(ipc_dir, mode) < 0)
        {
            log_error("cannot create %s: %s\n", ipc_dir, strerror(errno));
            return -1;
        }

        (void)chmod(ipc_dir, mode);
    }

    /*  Create the process running PID file */
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if ((fd = open(pid_file, O_RDWR | O_CREAT | O_TRUNC, mode)) >= 0)
    {
        char pid[PID_ASCII_SIZE];
        snprintf(pid, sizeof(pid), "%u\n", (unsigned)getpid());
        write(fd, pid, strlen(pid));
        close(fd);

        log_debug("Record PID<%u> to file %s.\n", getpid(), pid_file);
    }
    else
    {
        log_error("cannot create %s: %s\n", pid_file, strerror(errno));
        return -1;
    }

    return 0;
}

/* ****************************************************************************
 * FunctionName: get_daemon_pid
 * Description : Get the daemon process PID from the PID record file "pid_file"
 * Inputs      : pid_file: the PID record file
 * Output      : NONE
 * Return      : pid_t: The daemon process PID number
 * *****************************************************************************/
pid_t get_daemon_pid(const char *pid_file)
{
    FILE *f;
    pid_t pid;

    if ((f = fopen(pid_file, "rb")) != NULL)
    {
        char pid_ascii[PID_ASCII_SIZE];
        (void)fgets(pid_ascii, PID_ASCII_SIZE, f);
        (void)fclose(f);
        pid = atoi(pid_ascii);
    }
    else
    {
        log_error("Can't open PID record file %s: %s\n", pid_file, strerror(errno));
        return -1;
    }
    return pid;
}

/* ****************************************************************************
 * FunctionName: check_daemon_running
 * Description : Check the daemon program already running or not
 * Inputs      : pid_file: The record running daemon program PID
 * Output      : NONE
 * Return      : 1: The daemon program alread running   0: Not running
 * *****************************************************************************/
int check_daemon_running(const char *pid_file)
{
    int rv = -1;
    struct stat fStatBuf;

    rv = stat(pid_file, &fStatBuf);
    if (0 == rv)
    {
        pid_t pid = -1;
        printf("PID record file \"%s\" exist.\n", pid_file);

        pid = get_daemon_pid(pid_file);
        if (pid > 0)  /*  Process pid exist */
        {
            if ((rv = kill(pid, 0)) == 0)
            {
                printf("Program with PID[%d] seems running.\n", pid);
                return 1;
            }
            else   /* Send signal to the old process get no reply. */
            {
                printf("Program with PID[%d] seems exit.\n", pid);
                remove(pid_file);
                return 0;
            }
        }
        else if (0 == pid)
        {
            printf("Can not read program PID form record file.\n");
            remove(pid_file);
            return 0;
        }
        else  /* Read pid from file "pid_file" failure */
        {
            printf("Read record file \"%s\" failure, maybe program still running.\n", pid_file);
            return 1;
        }
    }

    return 0;
}

/* ****************************************************************************
 * FunctionName: stop_daemon_running
 * Description : Stop the daemon program running
 * Inputs      : pid_file: The record running daemon program PID
 * Output      : NONE
 * Return      : 1: The daemon program alread running   0: Not running
 * *****************************************************************************/
int stop_daemon_running(const char *pid_file)
{
    pid_t            pid = -1;
    struct stat      fStatBuf;

    if ( stat(pid_file, &fStatBuf) < 0)
        return 0;

    printf("PID record file \"%s\" exist.\n", pid_file);
    pid = get_daemon_pid(pid_file);
    if (pid > 0)  /*  Process pid exist */
    {
        while ( (kill(pid, 0) ) == 0)
        {
            kill(pid, SIGTERM);
            sleep(1);
        }

        remove(pid_file);
    }

    return 0;
}



/* ****************************************************************************
 * FunctionName: set_daemon_running
 * Description : Set the programe running as daemon if it's not running and record
 *               its PID to the pid_file.
 * Inputs      : pid_file: The record running daemon program PID
 * Output      : NONE
 * Return      : 0: Successfully. 1: Failure
 * *****************************************************************************/
int set_daemon_running(const char *pid_file)
{
    daemonize(0, 1);
    log_info("Program running as daemon [PID:%d].\n", getpid());

    if (record_daemon_pid(pid_file) < 0)
    {
        log_error("Record PID to file \"%s\" failure.\n", pid_file);
        return -2;
    }

    return 0;
}

/* start a new thread to run $thread_workbody point function  */
int thread_start(pthread_t *thread_id, thread_body_t thread_workbody, void *thread_arg)
{
    int                rv = 0;
    pthread_t          tid;

    pthread_attr_t     thread_attr;

    /* Initialize the thread  attribute */
    rv = pthread_attr_init(&thread_attr);
    if(rv)
        return -1;

    /* Set the stack size of the thread */
    rv = pthread_attr_setstacksize(&thread_attr, 120 * 1024);
    if(rv)
        goto CleanUp;

    /* Set thread to detached state:Don`t need pthread_join */
    rv = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if(rv)
        goto CleanUp;

    /* Create the thread */
    rv = pthread_create(&tid, &thread_attr, thread_workbody, thread_arg);
    if(rv)
        goto CleanUp;

CleanUp:


    if( thread_id )
    {
        if( rv )
            *thread_id = 0;
        else
            *thread_id = tid;
    }

    /* Destroy the  attributes  of  thread */
    pthread_attr_destroy(&thread_attr);
    return rv;
}


/* excute a linux command by system() */
void exec_system_cmd(const char *format, ...)
{
    char                cmd[256];
    va_list             args;

    memset(cmd, 0, sizeof(cmd));

    va_start(args, format);
    vsnprintf(cmd, sizeof(cmd), format, args);
    va_end(args);

    system(cmd);
}


