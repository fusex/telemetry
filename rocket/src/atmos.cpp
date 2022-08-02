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
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <ClosedCube_HDC1080.h>
#include <BGC_I2c.h>

#include "trame.h"

ClosedCube_HDC1080 hdc1080;
static Adafruit_BMP280 bmp;

void setupAtmos()
{
    if (!bmp.begin(BGC_I2C_PRESSURE_ADDR, 0x58)) {
        Init_SetFailed();
        return;
    }

   hdc1080.begin(BGC_I2C_TEMP_HUM_ADDR);
   if (hdc1080.readManufacturerId() != 0x5449 ||
           hdc1080.readDeviceId() != 0x1050) {
        Init_SetFailed();
        return;
    }

    TTRACE("init Done.\r\n");
}

void loopAtmos()
{
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
    float humidity = hdc1080.readHumidity();
    float temperature2 = hdc1080.readTemperature();

    fxtm_settemperature(temperature);
    fxtm_settemperature2(temperature2);
    fxtm_setpressure(pressure);
    fxtm_sethumidity(humidity);

    DTRACE("ZSK packet acquired in:%ld and prepared in %ld us\r\n", d1, micros()-time);
}
