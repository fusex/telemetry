#include <string.h>

#include "fusexcommon.h"
#include "real.h"

/* ICM20948 */
#define ACCEL_2G_RANGE_COEF  16.384 /* For +-2g */
#define ACCEL_4G_RANGE_COEF   8.192 /* For +-4g */
#define ACCEL_8G_RANGE_COEF   4.096 /* For +-8g */
#define ACCEL_16G_RANGE_COEF  2.048 /* For +-16g */

#define GYRO_DPS250_COEF     (131)
#define GYRO_DPS500_COEF     (65.5)
#define GYRO_DPS1000_COEF    (32.8)
#define GYRO_DPS2000_COEF    (16.4)

#define MAG_COEF             (0.15)

/* ICM20600 */
#define ACCEL2_2G_RANGE_COEF   4000 /* For +-2g */
#define ACCEL2_4G_RANGE_COEF   8000 /* For +-4g */
#define ACCEL2_8G_RANGE_COEF  16000 /* For +-8g */
#define ACCEL2_16G_RANGE_COEF 32000 /* For +-16g */

#if (ACCEL_RANGE == 16)
#define ACCEL_COEF      ACCEL_16G_RANGE_COEF
#else
#error
#endif

#if (ACCEL2_RANGE == 2)
#define ACCEL2_COEF     ACCEL2_2G_RANGE_COEF
#else
#error
#endif

#if (GYRO_RANGE == 500)
#define GYRO_COEF      GYRO_DPS500_COEF 
#else
#error
#endif

/* NeoGPS/Location */
#define GPS_COEF        (1.0e-7)

void fxreal_new ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;

    memset(rocket, 0, sizeof(fxtm_real_t));
    rocket->timestamp = tm->rxf.timestamp;
    rocket->id = tm->id;
}

void fxreal_setaccel ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;
    imuraw_t     a[3] = {0,0,0};

    IMU_SENSOR_GET(accel, tm, a[0], a[1], a[2]);

    rocket->accel.x = ((float)a[0])/ACCEL_COEF;
    rocket->accel.y = ((float)a[1])/ACCEL_COEF;
    rocket->accel.z = ((float)a[2])/ACCEL_COEF;
    rocket->accel.r = ACCEL_RANGE;
}

void fxreal_setgps ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;
    gpsraw_t     gps[2] = {0,0};

    fxtm_getabsgps(tm, &gps[0], &gps[1]);

    rocket->gps.lat = ((float)(gps[0])/GPS_COEF;
    rocket->gps.lon = ((float)(gps[1])/GPS_COEF;
}

void fxreal_setaccel2 ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;
    imuraw_t     a2[3] = {0,0,0};

    IMU_SENSOR_GET(accel2, tm, a2[0], a2[1], a2[2]);
    rocket->accel2.x = (((int32_t)a2[0]*ACCEL2_COEF)>>16);
    rocket->accel2.y = (((int32_t)a2[1]*ACCEL2_COEF)>>16);
    rocket->accel2.z = (((int32_t)a2[2]*ACCEL2_COEF)>>16);

    rocket->accel2.r = ACCEL2_RANGE;
}

void fxreal_setgyro ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;
    imuraw_t     g[3] = {0,0,0};

    IMU_SENSOR_GET(gyro, tm, g[0], g[1], g[2]);
    rocket->gyro.x = ((float)g[0])/GYRO_COEF;
    rocket->gyro.y = ((float)g[1])/GYRO_COEF;
    rocket->gyro.z = ((float)g[2])/GYRO_COEF;
}

void fxreal_setmagn ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;
    imuraw_t     m[3] = {0,0,0};

    IMU_SENSOR_GET(magn, tm, m[0], m[1], m[2]);
    rocket->magn.x = ((float)m[0])*MAG_COEF;
    rocket->magn.y = ((float)m[1])*MAG_COEF;
    rocket->magn.z = ((float)m[2])*MAG_COEF;
}

void fxreal_setatmos ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;

    rocket->atmos.pressure = tm->pressure;
    rocket->atmos.humidity = tm->humidity;
    rocket->atmos.temperature = tm->temperature;
}

void fxreal_setradio ()
{
    fxtm_rxfooter_t* rxf = &fxtmblock.rxf;
    fxtm_real_t*     rocket = &fxtmreal;

    rocket->radio.rssi = rxf->rssi;
    rocket->radio.snr  = rxf->snr;
    rocket->radio.frequencyError = rxf->frequencyError;
    rocket->radio.frequency = RADIO_FREQ;
}

void fxreal_setpitot ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;

    //TODO
    rocket->diffpressure = tm->diffpressure;
}

void fxreal_setflightstatus ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;

    rocket->flightStatus = FXTM_FLIGHTSTATUS(tm->flightStatus);
}

void fxreal_seterror ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;

    rocket->errors = FXTM_ERROR(tm->flightStatus);
}

void fxreal_setbattlevel ()
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_real_t* rocket = &fxtmreal;

    rocket->battLevel = tm->battLevel;
}

void fxreal_finish ()
{
    fxtm_real_t* rocket = &fxtmreal;

    rocket->altitude = 44330 *
        (1.0 - pow(rocket->atmos.pressure/PRESSURE_SEALEVEL_REF, 0.1903);

    //TODO:
    rocket->groundspeed.speedx = 0;
    rocket->groundspeed.speedy = 0;

    //TODO:
    rocket->orientation.qx = 0;
    rocket->orientation.qy = 0;
    rocket->orientation.qz = 0;
    rocket->orientation.qw = 0;

    //TODO:
    rocket->position.px = 0;
    rocket->position.py = 0;
    rocket->position.pz = 0;
}
