/*
* =====================================================================================
*
*       Filename:  atmos.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  16/01/2022 01:05:11
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
*        Company:  Derb.io 
*
* =====================================================================================
*/

#define TAG "ATMOS"

#include "init.h"
#include <fusexconfig.h>
#include <fusexutil.h>

#include "Wire.h"
#include <Adafruit_BMP280.h>

#include "BGC_Pinout.h"
#include "trame.h"

Adafruit_BMP280 bmp;
float   temperature;
float   pressure;

void setupAtmos()
{
    if (!bmp.begin(0x76, 0x58)) {
        Init_SetFailed();
        return;
    }

    TTRACE("init Done.\r\n");
}

void loopAtmos()
{
    temperature = bmp.readTemperature();
    pressure = bmp.readPressure();

    fxtm_settemperature(0, temperature);
    fxtm_setpressure(0, pressure);

    DTRACE("ZSK packet acquired in:%ld and prepared in %ld us\r\n", d1, micros()-time);
}