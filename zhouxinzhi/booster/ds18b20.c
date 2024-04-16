/*********************************************************************************
 *      Copyright:  (C) 2023 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.c
 *    Description:  This file is temperature sensor DS18B20 code
 *
 *        Version:  1.0.0(2023/8/10)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "2023/8/10 12:13:26"
 *
 * Pin connection:
 *
 *               DS18B20 Module          Raspberry Pi Board
 *                   VCC      <----->      #Pin1(3.3V)
 *                   DQ       <----->      #Pin7(BCM GPIO4)
 *                   GND      <----->      GND
 *
 * /boot/config.txt:
 *
 *          dtoverlay=w1-gpio-pullup,gpiopin=4
 *
 ********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "logger.h"

/* File Content:
   pi@raspberrypi:~/guowenxue $ cat /sys/bus/w1/devices/28-041731f7c0ff/w1_slave
   3a 01 4b 46 7f ff 0c 10 a5 : crc=a5 YES
   3a 01 4b 46 7f ff 0c 10 a5 t=19625
   */

int ds18b20_get_temperature(float *temp)
{
    char            w1_path[50] = "/sys/bus/w1/devices/";
    char            chip[20];
    char            buf[128];
    DIR            *dirp;
    struct dirent  *direntp;
    int             fd =-1;
    char           *ptr;
    int             found = 0;

    if( !temp )
    {
        return -1;
    }

    /*+-------------------------------------------------------------------+
     *|  open dierectory /sys/bus/w1/devices to get chipset Serial Number |
     *+-------------------------------------------------------------------+*/
    if((dirp = opendir(w1_path)) == NULL)
    {
        log_error("opendir error: %s\n", strerror(errno));
        return -2;
    }

    while((direntp = readdir(dirp)) != NULL)
    {
        if(strstr(direntp->d_name,"28-"))
        {
            /* find and get the chipset SN filename */
            strcpy(chip,direntp->d_name);
            found = 1;
            break;
        }
    }
    closedir(dirp);

    if( !found )
    {
        log_error("Can not find ds18b20 in %s\n", w1_path);
        return -3;
    }

    /* get DS18B20 sample file full path: /sys/bus/w1/devices/28-xxxx/w1_slave */
    strncat(w1_path, chip, sizeof(w1_path)-strlen(w1_path));
    strncat(w1_path, "/w1_slave", sizeof(w1_path)-strlen(w1_path));

    /* open file /sys/bus/w1/devices/28-xxxx/w1_slave to get temperature */
    if( (fd=open(w1_path, O_RDONLY)) < 0 )
    {
        log_error("open %s error: %s\n", w1_path, strerror(errno));
        return -4;
    }

    if(read(fd, buf, sizeof(buf)) < 0)
    {
        log_error("read %s error: %s\n", w1_path, strerror(errno));
        return -5;
    }

    ptr = strstr(buf, "t=");
    if( !ptr )
    {
        log_error("ERROR: Can not get temperature\n");
        return -6;
    }

    ptr+=2;

    /* convert string value to float value */
    *temp = atof(ptr)/1000;

    close(fd);

    return 0;
}
