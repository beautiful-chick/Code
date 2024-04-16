/********************************************************************************
 *      Copyright:  (C) 2022 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  packet.h
 *    Description:  This head file is packet API functions.
 *
 *        Version:  1.0.0(18/04/22)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "18/04/22 16:24:40"
 *
 ********************************************************************************/


#ifndef  _PACKET_H_
#define  _PACKET_H_

#include <stdint.h>
#include <time.h>

#define DEVID_LEN          16
#define TIME_LEN           32

typedef struct pack_info_s
{
    char          devid[DEVID_LEN];  /* device ID  */
    struct tm     sample_time;       /* sample time  */
    float         temper;            /* sample temperature */
} pack_info_t;

/* packet function pointer type */
typedef int (* pack_proc_t)(pack_info_t *pack_info, char *pack_buf, int size);

/*  description: get device ID
 *   input args:
 *               $devid :  device ID string
 *               $size  :  device ID output buffer size
 *               $sn    :  serial number
 * return value: <0: failure   0:ok
 */
extern int get_devid(char *devid, int size, int sn);

/*  description: get current system in struct tm
 *   input args:
 *               $sample_time:  sample time in struct tm
 * return value: <0: failure   0:ok
 */
extern int get_time(struct tm *sample_time);

/*  description: package a string packet in format "devid|time|temper"
 *   input args:
 *               $pack_info:  packet data contains devid, time and temperature
 *               $pack_buf :  packet output buffer
 *               $size     :  packet output buffer size
 * return value: <0: failure   >0: packet bytes
 */
extern int packet_segmented_pack(pack_info_t *pack_info, char *pack_buf, int size);


/*  description: package a json string packet: {"devid":"xxx", "time":"xxx", "temperature":"xxx"}
 *   input args:
 *               $pack_info:  packet data contains devid, time and temperature
 *               $pack_buf :  packet output buffer
 *               $size     :  packet output buffer size
 * return value: <0: failure   >0: packet bytes
 */
extern int packet_json_pack(pack_info_t *pack_info, char *pack_buf, int size);


#endif   /* ----- #ifndef _PACKET_H_  ----- */
