/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  log.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(18/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "18/03/24 00:51:41"
 *                 
 ********************************************************************************/

 
 
 

#include <stdarg.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <signal.h>  
#include <sys/time.h>  
#include <unistd.h>  
#include <time.h>  
#include "log.h"    
FILE *logfile = NULL;
LogLevel current_log_level = LOG_INFO;




void log_reopen() {
	if (logfile != NULL) {
		fclose(logfile); // 关闭当前的文件
		logfile = fopen("logfile.txt", "a"); // 以追加模式重新打开文件
		if (logfile == NULL) {
			perror("Error reopening log file");
			exit(EXIT_FAILURE); // 如果无法重新打开文件，则退出程序
		}
	}
}
void log_rollback_to_time(time_t rollback_time) {  
	if (logfile == NULL) {  
		fprintf(stderr, "Log file is not open\n");  
		return;  
	}  

	// 临时文件用于存储需要保留的日志条目  
	FILE *tmpfile = fopen("tmpfile.txt", "w");  
	if (tmpfile == NULL) {  
		perror("Failed to open temporary file");  
		exit(EXIT_FAILURE);  
	}  

	// 从原始日志文件中读取并过滤条目，只保留指定时间之后的条目  
	rewind(logfile); // 重置文件指针到文件开头  
	char line[1024];  
	time_t entry_time;  
	struct tm entry_tm;  

	while (fgets(line, sizeof(line), logfile)) {  
		// 假设日志条目的时间戳格式是 "[YYYY-MM-DD HH:MM:SS]"  
		if (sscanf(line, "[%d-%d-%d %d:%d:%d]",   
					&entry_tm.tm_year, &entry_tm.tm_mon, &entry_tm.tm_mday,  
					&entry_tm.tm_hour, &entry_tm.tm_min, &entry_tm.tm_sec) == 6) {  
			entry_tm.tm_year -= 1900; // 年份是从1900年开始计数的  
			entry_tm.tm_mon -= 1;     // 月份是从0开始的  
			entry_time = mktime(&entry_tm);  

			if (difftime(entry_time, rollback_time) >= 0) {  
				// 如果条目时间在回滚时间之后，则写入临时文件  
				fputs(line, tmpfile);  
			}  
		}  
	}  

	// 关闭并删除原始日志文件  
	fclose(logfile);  
	remove("logfile.txt");  

	// 将临时文件重命名为原始日志文件  
	fclose(tmpfile);  
	rename("tmpfile.txt", "logfile.txt");  

	// 重新打开日志文件  
	log_reopen();  
}

void log_rollback() {  
	// 假设我们想要回滚到一天前的时间  
	time_t one_day_ago = time(NULL) - 24 * 60 * 60; // 一天前的时间戳  
	log_rollback_to_time(one_day_ago);  
}  

void timer_handler(int signum) {  
	// 当定时器到期时，这里的代码会被执行  
	log_rollback();  
}  

void set_timer(int seconds) {  
	struct itimerval timer;  

	// 设置定时器间隔  
	timer.it_value.tv_sec = seconds; // 初始等待时间  
	timer.it_value.tv_usec = 0;  
	timer.it_interval.tv_sec = 0; // 不重复  
	timer.it_interval.tv_usec = 0;  

	// 设置信号处理器  
	signal(SIGALRM, timer_handler);  

	// 启动定时器  
	setitimer(ITIMER_REAL, &timer, NULL);  
}  
void log_init(const char *filename, LogLevel level)
{

	if(logfile != NULL)
	{

		fprintf(stderr, "LOg file is already open\n");
		sleep(3);
		return ;
	}
	logfile = fopen(filename, "a");
	if(logfile == NULL)
	{

		perror("Failed to open log file");
		exit(EXIT_FAILURE);
	}

	current_log_level = level;
	setvbuf(logfile, NULL, _IOLBF, 0);


}


void log_message(LogLevel level, const char *format,...)
{

	if(logfile == NULL || level > current_log_level)
	{
		return ;
	}

	time_t now;
	time(&now);
	char *level_str;
	switch(level)
	{
		case LOG_ERROR:
			level_str = "ERROR";
			break;
		case LOG_WARNING:
			level_str = "WARNING";
			break;
		case LOG_INFO:
			level_str = "INFO";
			break;
		case LOG_DEBUG:
			level_str = "DEBUG";
			break;
		default:
			level_str = "UNKNOWN";
	}

	struct tm *timeinfo = localtime(&now);
	char time_str[80];


	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

	va_list args;
	va_start(args, format);
	fprintf(logfile, "[%s] [%s]",time_str, level_str);
	vfprintf(logfile, format,args);
	fprintf(logfile,"\n");
	va_end(args);
	fflush(logfile); 







}
