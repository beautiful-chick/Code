/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  test_log.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 19:36:10"
 *                 
 ********************************************************************************/

#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "sys/stat.h"

void check_log_size_and_clear(const char *filepath,long max_size);

int main()
{
	const char *filepath = "server_log.txt";
	FILE 	   *file = fopen(filepath,"r");
	char 	   *line = NULL;
	size_t	   len = 0;
	ssize_t    read;

	if(file == NULL)
	{
		perror("Error opening file");
		return 1;
	}

	printf("日志文件 ‘%s’ 的内容：\n",filepath);

	while((read = getline(&line,&len,file)) != -1)
	{
		printf("%s\n",line);
	}

	check_log_size_and_clear(filepath,64*64);
	free(line);

	fclose(file);
	return 0;
}

void check_log_size_and_clear(const char *filepath, long max_size) //根据日志大小定空间删除日志内容
{
	struct stat st;

	if(stat(filepath,&st) == 0)
	{
		if(st.st_size >= max_size)
		{
			FILE *file = fopen(filepath,"w");

			if(file != NULL)
			{
				fclose(file);
			}
			else
			{
				printf("The log file is empty\n");
			}
		}

	}
}
