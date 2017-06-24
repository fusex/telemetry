/*
 * =====================================================================================
 *
 *       Filename:  gps.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:23:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#include <Arduino.h>
#define TAG "GPS"

#include <fusexutil.h>
#include <fusexconfig.h>

void setupGps()
{
    GPSdevice.begin(9600);
    while (!GPSdevice);

    TTRACE("init Done\r\n");
}

void loopGps()
{
#ifndef CONFIG_GPS
    return;
#endif

#if 1
 if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }
#endif

}
