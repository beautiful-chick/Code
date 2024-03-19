/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  daemon.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(03/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "03/03/24 11:01:33"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<syslog.h>
#include<libgen.h> /* basename() */

int main(int argc,char **argv)
{
	char	*progname = basename(argv[0]);/*截取文件名*/

	if(daemon(0,0) < 0)
	{
		printf("program daemon() failure: %s\n",strerror(errno));
		return -1;
	}

	openlog("daemon",LOG_CONS|LOG_PID,0);
	syslog(LOG_NOTICE,"program '%s' start running\n",progname);
	syslog(LOG_WARNING,"program '%s' running with a warnning message\n",progname);
	syslog(LOG_EMERG,"program '%s' running with a emergency message\n",progname);
	while(1)
	{
		;
	}
	syslog(LOG_NOTICE,"program '%s' stop running\n",progname);
	closelog();
	return 0;
}

