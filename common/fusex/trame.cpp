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

void fxtm_reset ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->id = idCounter++; 
    fxtmblock.timestamp = _mymillis();
}

void fxtm_setsoundlvl (unsigned int level)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->soundLevel = (uint8_t)(level/4);
}

void fxtm_setimu (float a[], float m[], float g[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    IMU_SENSOR_SET(accel, tm, (int32_t)(a[0]*IMUFACTOR), (int32_t)(a[1]*IMUFACTOR), (int32_t)(a[2]*IMUFACTOR));
    IMU_SENSOR_SET(gyro,   tm, (int32_t)(g[0]*IMUFACTOR), (int32_t)(g[1]*IMUFACTOR), (int32_t)(g[2]*IMUFACTOR));
    IMU_SENSOR_SET(magn,  tm, (int32_t)(m[0]*IMUFACTOR), (int32_t)(m[1]*IMUFACTOR), (int32_t)(m[2]*IMUFACTOR));
}

void fxtm_setimu2 (float a[], float g[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    IMU_SENSOR_SET(accel2, tm, (int32_t)(a[0]*IMUFACTOR), (int32_t)(a[1]*IMUFACTOR), (int32_t)(a[2]*IMUFACTOR));
    IMU_SENSOR_SET(gyro2,  tm, (int32_t)(g[0]*IMUFACTOR), (int32_t)(g[1]*IMUFACTOR), (int32_t)(g[2]*IMUFACTOR));
}

void fxtm_settemperature (uint8_t idx, float temperature)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->temperature[idx] = (int8_t) temperature;
}

void fxtm_setpressure (uint8_t idx, float pressure)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->pressure[idx] = (int)pressure;
}

void fxtm_setgps (float latitude, float longitude)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->gpsLt = (int32_t)(latitude*GPSFACTOR);
    tm->gpsLg = (int32_t)(longitude*GPSFACTOR);
}

fxtm_data_t* fxtm_getdata ()
{
    return &fxtmblock.data;
}

fxtm_block_t* fxtm_getblock ()
{
    return &fxtmblock;
}

size_t fxtm_getdatasize ()
{
    return sizeof(fxtm_data_t);
}

size_t fxtm_getblocksize ()
{
    return sizeof(fxtm_block_t);
}

void fxtm_getimu (fxtm_data_t* tm, float* imu)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;

    int32_t accel[3] = {0,0,0};
    int32_t magn[3]  = {0,0,0};
    int32_t gyro[3]   = {0,0,0};

    IMU_SENSOR_GET(accel, tm, accel[0], accel[1], accel[2]);
    IMU_SENSOR_GET(magn,  tm, magn[0],  magn[1],  magn[2]);
    IMU_SENSOR_GET(gyro,  tm, gyro[0],  gyro[1],  gyro[2]);

    imu[0] = (float)accel[0]/IMUFACTOR; 
    imu[1] = (float)accel[0]/IMUFACTOR; 
    imu[2] = (float)accel[0]/IMUFACTOR; 
    imu[3] = (float)gyro[0]/IMUFACTOR; 
    imu[4] = (float)gyro[1]/IMUFACTOR;
    imu[5] = (float)gyro[2]/IMUFACTOR; 
    imu[6] = (float)magn[0]/IMUFACTOR; 
    imu[7] = (float)magn[1]/IMUFACTOR;
    imu[8] = (float)magn[2]/IMUFACTOR;
}

void fxtm_getgps (fxtm_data_t* tm, float* gps)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;

    gps[0] = (float)tm->gpsLt/GPSFACTOR;
    gps[1] = (float)tm->gpsLg/GPSFACTOR;
}

void fxtm_getpressure (fxtm_data_t* tm, int32_t* ppressure)
{
    if(tm == NULL)
    	tm = &fxtmblock.data;

    *ppressure = tm->pressure;
}

void fxtm_getts (fxtm_data_t* tm, uint32_t* pts)
{
    *pts = fxtmblock.timestamp;
}

void fxtm_getid (fxtm_data_t* tm, uint16_t* pid)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;
    *pid = tm->id;
}

void fxtm_getsoundlvl (fxtm_data_t* tm, uint8_t* psndlvl)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;
    *psndlvl = tm->soundLevel;
}

void fxtm_gettemperature (fxtm_data_t* tm, int8_t* ptemp)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;
    *ptemp = tm->temperature;
}

uint16_t lastid = 0;
uint32_t lastts = 0;

int fxtm_check (fxtm_data_t* tm)
{
    if(tm == NULL)
	tm = &fxtmblock.data;

    int ret = 0;
    if (tm->id != (lastid +1) && tm->id != 0) {
	TTRACE("discontinuation at id: %u at ts: %lu, lastid:%u lastts:%lu\r\n",
	       tm->id, fxtmblock.timestamp, lastid, lastts);
	ret = 1;
    }
    lastid = tm->id;
    lastts = fxtmblock.timestamp;

    return ret;
}

void fxtm_dump (fxtm_data_t* tm)
{
    if(tm == NULL)
    	tm = &fxtmblock.data;

    int32_t accel[3]  = {0,0,0};
    int32_t gyro[3]    = {0,0,0};
    int32_t magn[3]   = {0,0,0};
    int32_t accel2[3] = {0,0,0};
    int32_t gyro2[3]   = {0,0,0};
   
    float gps[2] = {0,0};
    fxtm_getgps(tm, gps); 

    IMU_SENSOR_GET(accel, tm, accel[0], accel[1], accel[2]);
    IMU_SENSOR_GET(gyro,  tm, gyro[0],  gyro[1],  gyro[2]);
    IMU_SENSOR_GET(magn,  tm, magn[0],  magn[1],  magn[2]);
    IMU_SENSOR_GET(accel2, tm, accel[0], accel[1], accel[2]);
    IMU_SENSOR_GET(gyro2,  tm, gyro2[0], gyro2[1], gyro2[2]);

    TTRACE("\r\n\tid: %u at ts: %lu\r\n", tm->id, fxtmblock.timestamp);

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
    TRACE("\tsound level: %u\r\n",tm->soundLevel);
    TRACE("\ttemperature: %d C, pressure:%u pa\r\n", tm->temperature, tm->pressure);
    TRACE("\taccel[0]: %6ld, accel[1]: %6ld, accel[2]: %6ld\r\n", accel[0], accel[1], accel[2]);
    TRACE("\t gyro[0]: %6ld,  gyro[1]: %6ld,  gyro[2]: %6ld\r\n", gyro[0], gyro[1], gyro[2]);
    TRACE("\t magn[0]: %6ld,  magn[1]: %6ld,  magn[2]: %6ld\r\n", magn[0], magn[1], magn[2]);
    TRACE("\taccel2[0]: %6ld, accel2[1]: %6ld, accel2[2]: %6ld\r\n", accel2[0], accel2[1], accel2[2]);
    TRACE("\t gyro2[0]: %6ld,   gyr2[1]: %6ld,   gyr2[2]: %6ld\r\n", gyro2[0], gyro2[1], gyro2[2]);
}
