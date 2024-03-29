/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  debug.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(21/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "21/03/24 20:48:29"
 *                 
 ********************************************************************************/

#ifndef DEBUG_H

#define DEBUG_H
#include <stdio.h>

//#define CONFIG_DEBUG
#ifdef CONFIG_DEBUG

#define debug_printf(format,args...) printf(format,##args)

#else
#define debug_printf(format,args...) do{}while(0); 
#endif 

#endif 
