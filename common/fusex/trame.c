#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "trame.h"

#ifdef _IS_HOST
#include <cstdio>
#endif

static fxtm_block_t fxtmblock;
static uint16_t     idCounter = 0;

void fxtm_reset (uint32_t ts)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->id = idCounter++;
    memcpy(tm->magic, TRAME_VERSION, TRAME_VERSION_SIZE);
    fxtmblock.txh.timestamp = ts;
}

void fxtm_setimu (float a[], float m[], float g[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    IMU_SENSOR_SET(accel, tm, a[0], a[1], a[2]);
    IMU_SENSOR_SET(gyro,  tm, g[0], g[1], g[2]);
    IMU_SENSOR_SET(magn,  tm, m[0], m[1], m[2]);
}

void fxtm_setimu2 (float a[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    IMU_SENSOR_SET(accel2, tm, a[0], a[1], a[2]);
}

void fxtm_settemperature (int8_t temperature)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->temperature = temperature;
}

void fxtm_setpressure (uint16_t pressure)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->pressure = pressure;
}

void fxtm_setdiffpressure (uint16_t diffpressure)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->diffpressure = diffpressure;
}

void fxtm_sethumidity (uint8_t humidity)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->humidity = humidity;
}

void fxtm_setgps (float latitude, float longitude)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->gpsLt = latitude;
    tm->gpsLg = longitude;
}

void fxtm_setflightstatus (uint8_t flightStatus)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->flightStatus = flightStatus;
}

fxtm_txheader_t* fxtm_gettxheader (void)
{
    return &fxtmblock.txh;
}

fxtm_rxheader_t* fxtm_getrxheader (void)
{
    return &fxtmblock.rxh;
}

fxtm_data_t* fxtm_getdata (void)
{
    return &fxtmblock.data;
}

fxtm_block_t* fxtm_getblock ()
{
    return &fxtmblock;
}

size_t fxtm_getdatasize (void)
{
    return sizeof(fxtm_data_t);
}

size_t fxtm_gettxheadersize (void)
{
    return sizeof(fxtm_txheader_t);
}

size_t fxtm_getrxheadersize (void)
{
    return sizeof(fxtm_rxheader_t);
}

size_t fxtm_getrxdatasize (void)
{
    return fxtm_getdatasize() + fxtm_getrxheadersize();
}

size_t fxtm_gettxdatasize (void)
{
    return fxtm_gettxheadersize() + fxtm_getdatasize();
}

size_t fxtm_getblocksize (void)
{
    return sizeof(fxtm_block_t);
}

void fxtm_getimu (fxtm_data_t* tm, float imu[])
{
    if(tm == NULL)
    	tm = &fxtmblock.data;

    int32_t accel[3] = {0,0,0};
    int32_t magn[3]  = {0,0,0};
    int32_t gyro[3]  = {0,0,0};

    IMU_SENSOR_GET(accel, tm, accel[0], accel[1], accel[2]);
    IMU_SENSOR_GET(magn,  tm, magn[0],  magn[1],  magn[2]);
    IMU_SENSOR_GET(gyro,  tm, gyro[0],  gyro[1],  gyro[2]);

    imu[0] = accel[0];
    imu[1] = accel[0];
    imu[2] = accel[0];
    imu[3] = gyro[0];
    imu[4] = gyro[1];
    imu[5] = gyro[2];
    imu[6] = magn[0];
    imu[7] = magn[1];
    imu[8] = magn[2];
}

void fxtm_getgps (fxtm_data_t* tm, float gps[])
{
    if(tm == NULL)
        tm = &fxtmblock.data;

    gps[0] = tm->gpsLt;
    gps[1] = tm->gpsLg;
}

void fxtm_getpressure (fxtm_data_t* tm, uint16_t* ppressure)
{
    if(tm == NULL)
        tm = &fxtmblock.data;

    *ppressure = tm->pressure;
}

void fxtm_getts (fxtm_data_t* tm, uint32_t* pts)
{
    *pts = fxtmblock.txh.timestamp;
}

void fxtm_getid (fxtm_data_t* tm, uint16_t* pid)
{
    if(tm == NULL)
        tm = &fxtmblock.data;
    *pid = tm->id;
}

#if 0
void fxtm_getsoundlvl (fxtm_data_t* tm, uint8_t* psndlvl)
{
    if(tm == NULL)
        tm = &fxtmblock.data;
    *psndlvl = tm->soundLevel;
}

void fxtm_setsoundlvl (unsigned int level)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->soundLevel = (uint8_t)(level/4);
}
#endif

void fxtm_gettemperature (fxtm_data_t* tm, int8_t* ptemp)
{
    if(tm == NULL)
        tm = &fxtmblock.data;
    *ptemp = tm->temperature;
}

void fxtm_gethumidity (fxtm_data_t* tm, uint8_t* phumidity)
{
    if(tm == NULL)
        tm = &fxtmblock.data;
    *phumidity = tm->humidity;
}

void fxtm_getflightstatus (fxtm_data_t* tm, uint8_t* pFlightStatus)
{
    if(tm == NULL)
        tm = &fxtmblock.data;
    *pFlightStatus = tm->flightStatus;
}

static uint16_t lastid = 0;
static uint32_t lastts = 0;

uint16_t fxtm_check (fxtm_data_t* tm, uint16_t* plastid, uint32_t* plastts)
{
    uint16_t ret = 0;

    if (tm == NULL) {
        tm = &fxtmblock.data;
    }

    *plastts = lastts;
    *plastid = lastid;

    if (tm->id != (lastid +1) && tm->id != 0) {
        ret = lastid;
    }

    lastid = tm->id;
    lastts = fxtmblock.rxh.timestamp;

    return ret;
}

#define STRINGIFY(f, ...) do { \
    assert(remaining>0); \
    wrote = snprintf(buf+totalwrote, remaining, f,  ##__VA_ARGS__); \
    totalwrote += wrote; \
    remaining -= wrote;  \
} while(0);

size_t fxtm_dumpdata(fxtm_data_t* tm, char* buf, size_t bufsize)
{
    size_t wrote = 0;
    size_t totalwrote = 0;
    size_t remaining = bufsize;

    float   a[3] = {0,0,0};
    float   g[3] = {0,0,0};
    float   m[3] = {9,0,0};
    float  a2[3] = {0,0,0};
    float gps[2] = {0,0};

    fxtm_getgps(tm, gps);

    IMU_SENSOR_GET(accel,  tm, a[0],  a[1],  a[2]);
    IMU_SENSOR_GET(accel2, tm, a2[0], a2[1], a2[2]);
    IMU_SENSOR_GET(gyro,   tm, g[0],  g[1],  g[2]);
    IMU_SENSOR_GET(magn,   tm, m[0],  m[1],  m[2]);

    STRINGIFY("\r\n\tid: %u at ts: %u\r\n", tm->id, fxtmblock.txh.timestamp);
    STRINGIFY("\tgps: %f, %f\r\n", gps[0], gps[1]);

    STRINGIFY("\tflightstatus: %s (%3d)\r\n",
            tm->flightStatus==FXTM_FLIGHTSTATUS_LAUNCHPAD?"LAUNCHPAD":
            tm->flightStatus==FXTM_FLIGHTSTATUS_LIFTOFF?"LIFTOFF":
            tm->flightStatus==FXTM_FLIGHTSTATUS_BURNOUT?"BURNOUT":
            tm->flightStatus==FXTM_FLIGHTSTATUS_SEPARATION?"SEPARATION":
            tm->flightStatus==FXTM_FLIGHTSTATUS_RECOVERY?"RECOVERY":
            tm->flightStatus==FXTM_FLIGHTSTATUS_TOUCHDOWN?"TOUCHDOWN":
            "ERROR", tm->flightStatus);

#if 0
    STRINGIFY("\tsound level: %u\r\n",tm->soundLevel);
#endif
    STRINGIFY("\tpressure:%u pa, diffpressure:%u pa\r\n", tm->pressure, tm->diffpressure);
    STRINGIFY("\ttemperature: %d C, humidity:%u %%\r\n", tm->temperature, tm->humidity);
    STRINGIFY("\t accel[0]: %6f,  accel[1]: %f,   accel[2]: %6f\r\n", a[0], a[1], a[2]);
    STRINGIFY("\t  gyro[0]: %f,   gyro[1]: %f,    gyro[2]: %6f\r\n", g[0], g[1], g[2]);
    STRINGIFY("\t  magn[0]: %6f,   magn[1]: %6f,    magn[2]: %6f\r\n", m[0], m[1], m[2]);
    STRINGIFY("\taccel2[0]: %6f, accel2[1]: %6f,  accel2[2]: %6f\r\n", a2[0], a2[1], a2[2]);

    return totalwrote;
}

size_t fxtm_tojson(fxtm_data_t* tm, char* buf, size_t bufsize)
{
    float   a[3] = {0,0,0};
    float   g[3] = {0,0,0};
    float   m[3] = {9,0,0};
    float  a2[3] = {0,0,0};
    float gps[2] = {0,0};

    fxtm_getgps(tm, gps);
    IMU_SENSOR_GET(accel,  tm, a[0],  a[1],  a[2]);
    IMU_SENSOR_GET(accel2, tm, a2[0], a2[1], a2[2]);
    IMU_SENSOR_GET(gyro,   tm, g[0],  g[1],  g[2]);
    IMU_SENSOR_GET(magn,   tm, m[0],  m[1],  m[2]);

    size_t wrote = 0;
    size_t totalwrote = 0;
    size_t remaining = bufsize;

    STRINGIFY("{\"id\":%u, ", tm->id);
    STRINGIFY("\"pressure\":%u, \"diffpressure\":%u, ", tm->pressure, tm->diffpressure);
    STRINGIFY("\"temperature\":%d, \"humidity\":%u, ", tm->temperature, tm->humidity);
    STRINGIFY("\"longitude\":%f, \"latitude\":%f, ", gps[0], gps[1]);
    STRINGIFY("\"accelx\":%f, \"accely\":%f, \"accelz\":%f, ", a[0], a[1], a[2]);
    STRINGIFY("\"gyrox\":%f, \"gyroy\":%f, \"gyroz\":%f, ", g[0], g[1], g[2]);
    STRINGIFY("\"magnx\":%f, \"magny\":%f, \"magnz\":%f, ", m[0], m[1], m[2]);
    STRINGIFY("\"accel2x\":%f, \"accel2y\":%f, \"accel2z\":%f, ", a2[0], a2[1], a2[2]);
    STRINGIFY("\"flightstatus\":\"%s\"}",
            tm->flightStatus==FXTM_FLIGHTSTATUS_LAUNCHPAD?"LAUNCHPAD":
            tm->flightStatus==FXTM_FLIGHTSTATUS_LIFTOFF?"LIFTOFF":
            tm->flightStatus==FXTM_FLIGHTSTATUS_BURNOUT?"BURNOUT":
            tm->flightStatus==FXTM_FLIGHTSTATUS_SEPARATION?"SEPARATION":
            tm->flightStatus==FXTM_FLIGHTSTATUS_RECOVERY?"RECOVERY":
            tm->flightStatus==FXTM_FLIGHTSTATUS_TOUCHDOWN?"TOUCHDOWN":
            "ERROR");

    return totalwrote;
}
