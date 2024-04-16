/*********************************************************************************
 *      Copyright:  (C) 2022 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  packet.c
 *    Description:  This file is packet API functions
 *
 *        Version:  1.0.0(18/04/22)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "18/04/22 16:30:25"
 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "packet.h"
#include "logger.h"
#include "ds18b20.h"

int get_devid(char *devid, int size, int sn)
{
    if( !devid || size<DEVID_LEN )
    {
        log_error("Invalid input arugments\n");
        return -1;
    }

    memset(devid, 0, size);
    snprintf(devid, size, "rpi#%04d", sn);
    return 0;
}

int get_time(struct tm *ptm)
{
    if( !ptm )
    {
        log_error("Invalid input arugments\n");
        return -1;
    }

    time_t now = time(NULL);
    localtime_r(&now, ptm);

    return 0;
}

int packet_segmented_pack(pack_info_t *pack_info, char *pack_buf, int size)
{
    char              strtime[TIME_LEN] = {'\0'};
    struct tm        *ptm;

    if( !pack_info || !pack_buf || size<=0 )
    {
        log_error("Invalid input arguments\n");
        return -1;
    }

    ptm = &pack_info->sample_time;
    snprintf(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    memset(pack_buf, 0, size);
    snprintf(pack_buf, size, "%s|%s|%.3f", pack_info->devid, strtime, pack_info->temper);

    return strlen(pack_buf);
}

int packet_json_pack(pack_info_t *pack_info, char *pack_buf, int size)
{
    char              strtime[TIME_LEN] = {'\0'};
    struct tm        *ptm;

    if( !pack_info || !pack_buf || size<=0 )
    {
        log_error("Invalid input arguments\n");
        return -1;
    }

    ptm = &pack_info->sample_time;
    snprintf(strtime, sizeof(strtime), "%04d-%02d-%2d %02d:%02d:%02d",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    memset(pack_buf, 0, size);
    snprintf(pack_buf, size, "{\"devid\":\"%s\", \"time\":\"%s\",\"temperature\":\"%.3f\"}",
            pack_info->devid, strtime, pack_info->temper);

    return strlen(pack_buf);
}

