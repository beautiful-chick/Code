/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  string_handle.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "14/03/24 19:04:25"
 *                 
 ********************************************************************************/
#include "string_handle.h"
#include "temp.h"
/*
 *	函数名称: getdate_time
 *  返回值: int 返回值<0则代表失败，成功返回0:
 *  参数: 传入一个tm 类型的结构体指针 *get_tmchar 转化为一个字符串buf传送回去
 *  说明: 将tm类型结构体转化成特定形式的字符串给buf。
 */

int getdate_time(struct tm *get_tm,char* buf)
{
	if ( snprintf(buf,128,"%d-%d-%d %d:%d:%d",
				1900 + get_tm->tm_year,
				1 + get_tm->tm_mon,
				get_tm->tm_mday,
				get_tm->tm_hour,
				get_tm->tm_min,
				get_tm->tm_sec) < 0 ) 
	{   
		printf("convert date time error:%s\n",strerror(errno));
		return -1; 
	}   

	return 0;
}



/*
 *  函数名称: report_string
 *  返回值: int <0时即为出错，>0时为成功 
 *  参数: t_temp_measure *str：temp函数采集的温度的结构体。char *buf；返回的字符串
 *  说明: 用来将一个t_temp_measure类型的结构体转换成为一段字符串。
*/

int report_string(t_temp_measure *str,char *buf)
{
	snprintf(buf,256,"%s|%s|%.2f",
			     str->dev_name,str->ptime,str->temp);

	return 0;
}

/*
 *	函数名称: 
 *  返回值: int 返回值<0则代表失败，成功返回0:
 *  参数: char *str: 需要分割的字符串 char cut_mark: 分割的符号 char* *buf: 字符串数组，用来存储结果。 
 *  说明:     
 */
int apart_string(char *str,char* cut_mark,void* buf) 
{
	t_temp_measure				*rebuf = NULL;
	char 					    *tmp = NULL;

	rebuf = (t_temp_measure *)buf;

	tmp = strtok(str,cut_mark);
	snprintf(rebuf->dev_name,sizeof(rebuf->dev_name),"%s",tmp);

	tmp = strtok(NULL,cut_mark);
	snprintf(rebuf->ptime,sizeof(rebuf->ptime),"%s",tmp);

	tmp = strtok(NULL,cut_mark);
	rebuf->temp = atof(tmp);

	buf = rebuf;

	return 0;

}

