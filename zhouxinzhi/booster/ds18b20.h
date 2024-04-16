/*********************************************************************************
 *      Copyright:  (C) 2023 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.h
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

#ifndef  _DS18B20_H_
#define  _DS18B20_H_

extern int ds18b20_get_temperature(float *temp);

#endif   /* ----- #ifndef _DS18B20_H_  ----- */

