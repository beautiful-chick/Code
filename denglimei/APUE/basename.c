/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  basename.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "20/03/24 17:49:36"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <libgen.h>


int main(int argc, char **argv)
{
	char   *progname = NULL;
	progname = basename(argv[0]);
	printf("Progname %s \n",progname);
	return 0;
}




