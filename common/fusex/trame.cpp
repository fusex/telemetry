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
    tm->soundlvl = (uint8_t)(level/4);
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

void fxtm_getimu(float* imu)
{
    fxtm_data_t* tm = &fxtmblock.data;

    int32_t accel[3] = {0,0,0};
    int32_t magn[3]  = {0,0,0};
    int32_t gyr[3]   = {0,0,0};

    GETT(accel, tm, accel[0], accel[1], accel[2]);
    GETT(magn,  tm, magn[0],  magn[1],  magn[2]);
    GETT(gyr,   tm, gyr[0],   gyr[1],   gyr[2]);

    imu[0] = (float)accel[0]/IMUFACTOR; 
    imu[1] = (float)accel[0]/IMUFACTOR; 
    imu[2] = (float)accel[0]/IMUFACTOR; 
    imu[3] = (float)gyr[0]/IMUFACTOR; 
    imu[4] = (float)gyr[1]/IMUFACTOR;
    imu[5] = (float)gyr[2]/IMUFACTOR; 
    imu[6] = (float)magn[0]/IMUFACTOR; 
    imu[7] = (float)magn[1]/IMUFACTOR;
    imu[8] = (float)magn[2]/IMUFACTOR;
}

void fxtm_getgps(float* gps)
{
    fxtm_data_t* tm = &fxtmblock.data;

    gps[0] = (float)tm->gpslt/GPSFACTOR;
    gps[1] = (float)tm->gpslg/GPSFACTOR;
}

void fxtm_getpressure(int32_t* ppressure)
{
    fxtm_data_t* tm = &fxtmblock.data;

    *ppressure = GET_PRESSURE(tm->rawpressure);
}

void fxtm_getts(uint32_t* pts)
{
    fxtm_data_t* tm = &fxtmblock.data;
    *pts = tm->timestamp;
}

void fxtm_getid(uint16_t* pid)
{
    fxtm_data_t* tm = &fxtmblock.data;
    *pid = tm->id;
}

void fxtm_getsoundlvl(uint8_t* psndlvl)
{
    fxtm_data_t* tm = &fxtmblock.data;
    *psndlvl = tm->soundlvl;
}

void fxtm_gettemperature(int8_t* ptemp)
{
    fxtm_data_t* tm = &fxtmblock.data;
    *ptemp = tm->temperature;
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
   
    float gps[2] = {0,0};

    fxtm_getgps(gps); 

    GETT(accel, tm, accel[0], accel[1], accel[2]);
    GETT(magn,  tm, magn[0],  magn[1],  magn[2]);
    GETT(gyr,   tm, gyr[0],   gyr[1],   gyr[2]);

    TTRACE("\r\n\tid: %u at ts: %lu\r\n", tm->id, tm->timestamp);
#if _IS_PC 
    TRACE("\tgps: %f,%f\r\n",gps[0], gps[1]);
#else
    //TRACE("\tgps: %ld.%ld,%ld.%ld\r\n",abs((int)gps[0]), (int)(gps[0]*GPSFACTOR)%GPSFACTOR, abs((int)gps[1]),(int)(gps[1]*GPSFACTOR)%GPSFACTOR);
{
    long   i,d;
    double u;

    i = abs((int)gps[0]);
    u = (double)(gps[0]-i);
    if (gps[0]<0) u = -u;
    d = u*GPSFACTOR;

    TRACE("\tgps: %s%ld.%0" GPSFACTORPOW "ld, ", gps[0]<0?"-":"", i, d);
}
{
    long   i,d;
    double u;

    i = abs((int)gps[1]);
    u = (double)(gps[1]-i);
    if (gps[1]<0) u = -u;
    d = u*GPSFACTOR;

    TRACE("%s%ld.%0" GPSFACTORPOW "ld\r\n", gps[1]<0?"-":"", i, d);
}
#endif
    TRACE("\tsound level: %u\r\n",tm->soundlvl);
    TRACE("\ttemperature: %d C, rawpressure:%u pressure:%lu pa\r\n",
	  tm->temperature, tm->rawpressure, GET_PRESSURE(tm->rawpressure));

    TRACE("\taccel[0]: %6ld, accel[1]: %6ld, accel[2]: %6ld\r\n", accel[0], accel[1], accel[2]);
    TRACE("\t  gyr[0]: %6ld,   gyr[1]: %6ld,   gyr[2]: %6ld\r\n", gyr[0], gyr[1], gyr[2]);
    TRACE("\t magn[0]: %6ld,  magn[1]: %6ld,  magn[2]: %6ld\r\n", magn[0], magn[1], magn[2]);
}
