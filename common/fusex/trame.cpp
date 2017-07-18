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

#include <stddef.h>
#include "fusexutil.h"
#include "trame.h"

fxtm_block_t  fxtmblock;
uint16_t      idCounter = 0;

void fxtm_reset()
{
    fxtm_data_t* tm = &fxtmblock.data;

    tm->timestamp = _mymillis();
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
    SETT(accel, tm, (int32_t)(a[0]*IMUFACTOR), (int32_t)(a[1]*IMUFACTOR), (int32_t)(a[2]*IMUFACTOR));
    SETT(magn,  tm, (int32_t)(m[0]*IMUFACTOR), (int32_t)(m[1]*IMUFACTOR), (int32_t)(m[2]*IMUFACTOR));
    SETT(gyr,   tm, (int32_t)(g[0]*IMUFACTOR), (int32_t)(g[1]*IMUFACTOR), (int32_t)(g[2]*IMUFACTOR));
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

    tm->gpslt = (int32_t)(latitude*GPSFACTOR);
    tm->gpslg = (int32_t)(longitude*GPSFACTOR);
}

fxtm_data_t* fxtm_getdata()
{
    return &fxtmblock.data;
}

fxtm_block_t* fxtm_getblock()
{
    return &fxtmblock;
}

size_t fxtm_getdatasize()
{
    return sizeof(fxtm_data_t);
}

size_t fxtm_getblocksize()
{
    return sizeof(fxtm_block_t);
}

uint16_t lastid = 0;
uint32_t lastts = 0;

int fxtm_check()
{
    fxtm_data_t* tm = fxtm_getdata();
    int ret = 0;
    if (tm->id != (lastid +1) && tm->id != 0) {
	TTRACE("discontinuation at id: %u at ts: %lu, lastid:%u lastts:%lu\r\n",
	       tm->id, tm->timestamp, lastid, lastts);
	ret = 1;
    }
    lastid = tm->id;
    lastts = tm->timestamp;

    return ret;
}

void fxtm_dump(fxtm_data_t* tm)
{
    if(tm == NULL)
	tm = &fxtmblock.data;

    int32_t accel[3] = {0,0,0};
    int32_t magn[3]  = {0,0,0};
    int32_t gyr[3]   = {0,0,0};

    GETT(accel, tm, accel[0], accel[1], accel[2]);
    GETT(magn,  tm, magn[0],  magn[1],  magn[2]);
    GETT(gyr,   tm, gyr[0],   gyr[1],   gyr[2]);

    TTRACE("\r\n\tid: %u at ts: %lu\r\n", tm->id, tm->timestamp);
#if _IS_PC 
    TRACE("\tgps: %d,%d\r\n",tm->gpslt, tm->gpslg);
#else
    TRACE("\tgps: %ld,%ld\r\n",tm->gpslt, tm->gpslg);
#endif
    TRACE("\tsound level: %u\r\n",tm->soundlvl);
    TRACE("\ttemperature: %d C, rawpressure:%u pressure:%lu pa\r\n",
	  tm->temperature, tm->rawpressure, GET_PRESSURE(tm->rawpressure));

    TRACE("\taccel[0]: %6ld, accel[1]: %6ld, accel[2]: %6ld\r\n", accel[0], accel[1], accel[2]);
    TRACE("\t  gyr[0]: %6ld,   gyr[1]: %6ld,   gyr[2]: %6ld\r\n", gyr[0], gyr[1], gyr[2]);
    TRACE("\t magn[0]: %6ld,  magn[1]: %6ld,  magn[2]: %6ld\r\n", magn[0], magn[1], magn[2]);
}
