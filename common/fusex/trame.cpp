/*
 * =====================================================================================
 *
 *       Filename:  trame.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  30/06/2017 22:51:15
 *       Revision:  none
 *       Compiler:  gcc
 *
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@derbsellicon.com
*        Company:  Derb.io 
 *
 * =====================================================================================
 */
#define TAG "FXTM"

#include <fusexutil.h>
#include "trame.h"

fxtm_block_t  fxtmblock;
uint16_t      idCounter = 0;

void fxtm_reset()
{
    fxtm_data_t* tm = &fxtmblock.data;

    tm->timestamp = millis();
    tm->id        = idCounter++; 
}

void fxtm_setsoundlvl(unsigned int level)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->soundlvl = (uint8_t)level; 
}

void fxtm_setimu(float a[], float m[], float g[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    SETT(accel, tm, (int16_t)a[0], (int16_t)a[1], (int16_t)a[2]);
    SETT(magn,  tm, (int16_t)m[0], (int16_t)m[1], (int16_t)m[2]);
    SETT(gyr,   tm, (int16_t)g[0], (int16_t)g[1], (int16_t)g[2]);
}

void fxtm_settemperature(float temperature)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->temperature = (int8_t) temperature;
}

void fxtm_setpressure(float pressure)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->rawpressure = GET_RAWPRESSURE((int)pressure);
}

void fxtm_setgps(float latitude, float longitude)
{
    fxtm_data_t* tm = &fxtmblock.data;

    tm->gpslt = (int32_t)(latitude*100000);
    tm->gpslg = (int32_t)(longitude*100000);
}

fxtm_data_t* fxtm_getdata()
{
    return &fxtmblock.data;
}

fxtm_block_t* fxtm_getblock()
{
    return &fxtmblock;
}

size_t fxtm_getsize()
{
    return sizeof(fxtm_data_t);
}

int m = 0;
void fxtm_dump()
{
    if(m++<50)
	return;
    m=0;

    fxtm_data_t* tm = &fxtmblock.data;

    int32_t accel[3] = {0,0,0};
    int32_t magn[3]  = {0,0,0};
    int32_t gyr[3]   = {0,0,0};

    GETT(accel, tm, accel[0], accel[1], accel[2]);
    GETT(magn,  tm, magn[0],  magn[1],  magn[2]);
    GETT(gyr,   tm, gyr[0],   gyr[1],   gyr[2]);

    TTRACE("\r\n\tid: %u at ts: %lu\r\n", tm->id, tm->timestamp);
    TRACE("\tgps: %ld,%ld\r\n",tm->gpslt, tm->gpslg);
    TRACE("\tsound level: %u\r\n",tm->soundlvl);
    TRACE("\ttemperature: %d C, rawpressure:%u pressure:%lu pa\r\n",
	  tm->temperature, tm->rawpressure, GET_PRESSURE(tm->rawpressure));

    TRACE("\taccel[0]: %6ld, accel[1]: %6ld, accel[2]: %6ld\r\n", accel[0], accel[1], accel[2]);
    TRACE("\t magn[0]: %6ld,  magn[1]: %6ld,  magn[2]: %6ld\r\n", magn[0], magn[1], magn[2]);
    TRACE("\t  gyr[0]: %6ld,   gyr[1]: %6ld,   gyr[2]: %6ld\r\n", gyr[0], gyr[1], gyr[2]);
}
