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

static fxtm_block_t fxtmblock;
static uint16_t idCounter = 0;

void fxtm_gendata ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    gendata((uint8_t*)tm, sizeof(fxtm_data_t));
}

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

void fxtm_setimu2 (float a[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    IMU_SENSOR_SET(accel2, tm, (int32_t)(a[0]*IMUFACTOR), (int32_t)(a[1]*IMUFACTOR), (int32_t)(a[2]*IMUFACTOR));
}

void fxtm_settemperature (float temperature)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->temperature = (int8_t) temperature;
}

void fxtm_setpressure (float pressure)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->pressure = (int)pressure;
}

void fxtm_sethumidity (float humidity)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->humidity = (uint8_t) humidity;
}

void fxtm_settemperature2 (float temp)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->temperature2 = (int8_t)temp;
}

void fxtm_setgps (float latitude, float longitude)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->gpsLt = (int32_t)(latitude*GPSFACTOR);
    tm->gpsLg = (int32_t)(longitude*GPSFACTOR);
}

void fxtm_setseparation (bool separated)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->separation = separated;
}

fxtm_data_t* fxtm_getdata ()
{
    return &fxtmblock.data;
}

fxtm_block_t* fxtm_getblock ()
{
    return &fxtmblock;
}

unsigned int fxtm_getdatasize ()
{
    return sizeof(fxtm_data_t);
}

size_t fxtm_getblocksize ()
{
    return sizeof(fxtm_block_t);
}

void fxtm_getimu (fxtm_data_t* tm, float imu[])
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

void fxtm_getgps (fxtm_data_t* tm, float gps[])
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

void fxtm_gethumidity (fxtm_data_t* tm, int8_t* phumidity)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;
    *phumidity = tm->humidity;
}

void fxtm_gettemperature2 (fxtm_data_t* tm, int8_t* ptemp)
{
    if(tm == NULL)
	    tm = &fxtmblock.data;
    *ptemp = tm->temperature2;
}

static uint16_t lastid = 0;
static uint32_t lastts = 0;

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

void fxtm_dump ()
{
    fxtm_data_t* tm = &fxtmblock.data;

    int32_t a[3]  = {0,0,0};
    int32_t g[3]  = {0,0,0};
    int32_t m[3]  = {9,0,0};
    int32_t a2[3] = {0,0,0};
    int32_t g2[3] = {0,0,0};
   
    float gps[2] = {0,0};
    fxtm_getgps(tm, gps); 

    IMU_SENSOR_GET(accel,  tm, a[0],  a[1],  a[2]);
    IMU_SENSOR_GET(accel2, tm, a2[0], a2[1], a2[2]);
    IMU_SENSOR_GET(gyro,   tm, g[0],  g[1],  g[2]);
    IMU_SENSOR_GET(magn,   tm, m[0],  m[1],  m[2]);

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
    TRACE("\tSeperation: %s\r\n",tm->separation?"ACTED":"NOT YET");
    TRACE("\tsound level: %u\r\n",tm->soundLevel);
    TRACE("\ttemperature: %d C, pressure:%u pa, diffpressure:%u pa\r\n",
	  tm->temperature, tm->pressure, tm->diffpressure);
    TRACE("\ttemperature2: %d C, humidity:%u %%\r\n", tm->temperature2, tm->humidity);
    TRACE("\t accel[0]: %6ld,  accel[1]: %6ld,   accel[2]: %6ld\r\n", a[0], a[1], a[2]);
    TRACE("\t  gyro[0]: %6ld,   gyro[1]: %6ld,    gyro[2]: %6ld\r\n", g[0], g[1], g[2]);
    TRACE("\t  magn[0]: %6ld,   magn[1]: %6ld,    magn[2]: %6ld\r\n", m[0], m[1], m[2]);
    TRACE("\taccel2[0]: %6ld, accel2[1]: %6ld,  accel2[2]: %6ld\r\n", a2[0], a2[1], a2[2]);
}
