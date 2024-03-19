/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  memory_layout.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(27/02/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "27/02/24 10:17:45"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<stdlib.h>

int g_var1;//未初始化的全局变量，存放在BSS区，值为0
int g_var2 = 20;//初始化的全局变量存在data区 值为20

int main(int argc,char **argv)//argv是命令行参数，存放在命令行参数区
{
	static int 		s_var1;//未初始化的静态变量，存在bss区 值为0
	static int 		s_var2 = 10;//初始化的静态变量 存在data区 值为10 
	char   			*str = "Hello";//str是初始化的局部变量，存放在栈中，它的值是“Hello”这个字符串常量 存放在DATA中的RODATA区中的地址
	char			*ptr;//未初始化的局部变量 存放在栈中，野指针
	ptr = malloc(100);//malloc（）会从堆中分配100个字节的内存空间，并将该内存空间的首地址返回给ptr存放

	printf("[cmd args] : argv address : %p\n",argv);
	printf("\n");
	printf("[Stack] : str address : %p\n",&str);
	printf("[Stack] : ptr address : %p\n",&ptr);
	printf("\n");

	printf("[ Heap ] : malloc address : %p\n",ptr);
	printf("\n");

	printf("[ bss ] : s_var1 address : %p value : %d\n",&s_var1,g_var1);
	printf("[ bss ] : g_var1 address : %p\n value : %d\n",&g_var1,g_var1);
	printf("\n");
	printf("[ data ] : g_var2 address : %p value : %d\n",&g_var2,g_var2);
	printf("[ data ] : s_var2 address  :%p value : %d\n",&s_var2,s_var2);
	printf("\n");
	printf("[ rodata ] : \"%s\" address : %p\n",str,str);
	printf("\n");
	printf("[Text] : main() address : %p\n",main);
		return 0;
}

