/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  daemon.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "10/03/24 17:38:30"
 *                 
 ********************************************************************************/

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char      *progname = basename(argv[0]);

	if( daemon(0, 0) < 0 )
	{
		printf("Program daemon() failure: %s\n",strerror(errno));
		return -1;
	}

	openlog("daemon",LOG_CONS | LOG_PID, 0);
	syslog(LOG_NOTICE, "Program '%s' start running \n",progname);
	syslog(LOG_WARNING, "Program '%s' running with a warning message\n",progname);
	syslog(LOG_EMERG, "Program '%s' running with a emergency message\n",progname);
	while(1)
	{
		;
	}

	syslog(LOG_NOTICE, "Program '%s' stop running \n",progname);
	closelog();

	return 0;
}







