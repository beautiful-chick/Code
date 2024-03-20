/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  log.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(18/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "18/03/24 23:32:11"
 *                 
 ********************************************************************************/

 



// logger.h  
#ifndef LOGGER_H  
#define LOGGER_H  
  
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
  
// 定义日志级别枚举  
typedef enum {  
    LOG_ERROR,    // 错误级别  
    LOG_WARNING,  // 警告级别  
    LOG_INFO,     // 信息级别  
    LOG_DEBUG,    // 调试级别  
    LOG_NONE      // 不记录任何日志  
} LogLevel;  


extern FILE *logfile;
// 声明全局变量，用于存储当前的日志级别  
extern LogLevel current_log_level;  
  
// 初始化日志系统函数声明  
void log_init(const char *filename, LogLevel level);  
  
// 记录日志函数声明  
void log_message(LogLevel level, const char *format, ...);  
void log_message(LogLevel level, const char *format,...); 
#endif // LOGGER_H





