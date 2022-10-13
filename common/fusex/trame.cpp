#define TAG "FXTM"

#include <stdlib.h>
#include <stddef.h>

#if !defined(_IS_PC)
# include <fusexutil.h>
#else
# include <fusexutilpc.h>
#endif

#include "trame.h"

static fxtm_block_t fxtmblock;

#if !defined(_IS_PC)
static uint16_t     idCounter = 0;

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
#endif

void fxtm_setimu (float a[], float m[], float g[])
{
    fxtm_data_t* tm = &fxtmblock.data;
    IMU_SENSOR_SET(accel, tm, (int32_t)(a[0]*IMUFACTOR), (int32_t)(a[1]*IMUFACTOR), (int32_t)(a[2]*IMUFACTOR));
    IMU_SENSOR_SET(gyro,  tm, (int32_t)(g[0]*IMUFACTOR), (int32_t)(g[1]*IMUFACTOR), (int32_t)(g[2]*IMUFACTOR));
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
    tm->pressure = (uint16_t)pressure;
}

void fxtm_setdiffpressure (uint16_t diffpressure)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->diffpressure = (uint16_t)diffpressure;
}

void fxtm_sethumidity (float humidity)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->humidity = (uint8_t) humidity;
}

void fxtm_setgps (float latitude, float longitude)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->gpsLt = (int32_t)(latitude*GPSFACTOR);
    tm->gpsLg = (int32_t)(longitude*GPSFACTOR);
}

void fxtm_setflightstatus (uint8_t flightStatus)
{
    fxtm_data_t* tm = &fxtmblock.data;
    tm->flightStatus = flightStatus;
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

void fxtm_gethumidity (fxtm_data_t* tm, int8_t* phumidity)
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

int fxtm_check (fxtm_data_t* tm)
{
    if(tm == NULL)
	tm = &fxtmblock.data;

    int ret = 0;
    if (tm->id != (lastid +1) && tm->id != 0) {
	TTRACE("discontinuation at id: %u at ts: %u, lastid:%u lastts:%u\r\n",
	       tm->id, fxtmblock.timestamp, lastid, lastts);
	ret = 1;
    }
    lastid = tm->id;
    lastts = fxtmblock.timestamp;

    return ret;
}

void fxtm_dump (bool isConsole)
{
    fxtm_data_t* tm = &fxtmblock.data;
    fxtm_dumpdata(tm);
}

void fxtm_dumpdata(fxtm_data_t* tm)
{
    int32_t a[3]  = {0,0,0};
    int32_t g[3]  = {0,0,0};
    int32_t m[3]  = {9,0,0};
    int32_t a2[3] = {0,0,0};
   
    float gps[2] = {0,0};
    fxtm_getgps(tm, gps); 

    IMU_SENSOR_GET(accel,  tm, a[0],  a[1],  a[2]);
    IMU_SENSOR_GET(accel2, tm, a2[0], a2[1], a2[2]);
    IMU_SENSOR_GET(gyro,   tm, g[0],  g[1],  g[2]);
    IMU_SENSOR_GET(magn,   tm, m[0],  m[1],  m[2]);

    MYTRACE("\r\n\tid: %u at ts: %u\r\n", tm->id, fxtmblock.timestamp);
{
    long   i,d;
    double u;

    i = abs((int)gps[0]);
    u = (double)(gps[0]-i);
    if (gps[0]<0) u = -u;
    d = u*GPSFACTOR;

    MYTRACE("\tgps: %s%ld.%0" GPSFACTORPOW "ld, ", gps[0]<0?"-":"", i, d);
}
{
    long   i,d;
    double u;

    i = abs((int)gps[1]);
    u = (double)(gps[1]-i);
    if (gps[1]<0) u = -u;
    d = u*GPSFACTOR;

    MYTRACE("%s%ld.%0" GPSFACTORPOW "ld\r\n", gps[1]<0?"-":"", i, d);
}
{
    MYTRACE("\tFlight Status: %s (%3d)\r\n",
	    tm->flightStatus==FXTM_FLIGHTSTATUS_LAUNCHPAD?"LAUNCHPAD":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_LIFTOFF?"LIFTOFF":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_BURNOUT?"BURNOUT":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_SEPARATION?"SEPARATION":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_RECOVERY?"RECOVERY":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_TOUCHDOWN?"TOUCHDOWN":
            "ERROR", tm->flightStatus);
}

#if 0
    MYTRACE("\tsound level: %u\r\n",tm->soundLevel);
#endif
    MYTRACE("\tpressure:%u pa, diffpressure:%u pa\r\n", tm->pressure, tm->diffpressure);
    MYTRACE("\ttemperature: %d C, humidity:%u %%\r\n", tm->temperature, tm->humidity);
    MYTRACE("\t accel[0]: %6d,  accel[1]: %6d,   accel[2]: %6d\r\n", a[0], a[1], a[2]);
    MYTRACE("\t  gyro[0]: %6d,   gyro[1]: %6d,    gyro[2]: %6d\r\n", g[0], g[1], g[2]);
    MYTRACE("\t  magn[0]: %6d,   magn[1]: %6d,    magn[2]: %6d\r\n", m[0], m[1], m[2]);
    MYTRACE("\taccel2[0]: %6d, accel2[1]: %6d,  accel2[2]: %6d\r\n", a2[0], a2[1], a2[2]);
}

#define GETGPS(x) do { \
    long   i,d;        \
    double u;          \
    i = abs((int)x);   \
    u = (double)(x-i); \
    if (x<0) u = -u;   \
    d = u*GPSFACTOR;   \
} while(0)

#define STRINGIFY(f, ...) do { \
    wrote = snprintf(buf+totalwrote, remaining, f,  ##__VA_ARGS__); \
    totalwrote += wrote; \
    remaining -= wrote;  \
} while(0);

size_t fxtm_tojson(fxtm_data_t* tm, char* buf, size_t bufsize)
{
    int32_t a[3]  = {0,0,0};
    int32_t g[3]  = {0,0,0};
    int32_t m[3]  = {9,0,0};
    int32_t a2[3] = {0,0,0};
    float   gps[2] = {0,0};

    fxtm_getgps(tm, gps);
    IMU_SENSOR_GET(accel,  tm, a[0],  a[1],  a[2]);
    IMU_SENSOR_GET(accel2, tm, a2[0], a2[1], a2[2]);
    IMU_SENSOR_GET(gyro,   tm, g[0],  g[1],  g[2]);
    IMU_SENSOR_GET(magn,   tm, m[0],  m[1],  m[2]);

    size_t wrote = 0;
    size_t totalwrote = 0;
    size_t remaining = bufsize;

    STRINGIFY("{id:%u, ", tm->id);
    STRINGIFY("pressure:%u, diffpressure:%u, ", tm->pressure, tm->diffpressure);
    STRINGIFY("temperature:%d, humidity:%u, ", tm->temperature, tm->humidity);
    STRINGIFY("longitude:%f, latitude:%f, ", gps[0], gps[1]);
    STRINGIFY("accelx:%d, accely:%d, accelz:%d, ", a[0], a[1], a[2]);
    STRINGIFY("gyrox:%d, gyroy:%d, gyroz:%d, ", g[0], g[1], g[2]);
    STRINGIFY("magnx:%d, magny:%d, magnz:%d, ", m[0], m[1], m[2]);
    STRINGIFY("accel2x:%d, accel2y:%d, accel2z:%d, ", a2[0], a2[1], a2[2]);
    STRINGIFY("flightstatus:%s}",
	    tm->flightStatus==FXTM_FLIGHTSTATUS_LAUNCHPAD?"LAUNCHPAD":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_LIFTOFF?"LIFTOFF":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_BURNOUT?"BURNOUT":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_SEPARATION?"SEPARATION":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_RECOVERY?"RECOVERY":
	    tm->flightStatus==FXTM_FLIGHTSTATUS_TOUCHDOWN?"TOUCHDOWN":
            "ERROR");

    return totalwrote;
}
