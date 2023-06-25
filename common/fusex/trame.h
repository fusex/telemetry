#ifndef _TRAME_H
#define _TRAME_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{

#define myaccum
#else
#define myaccum _Accum
#endif /* __cplusplus */

#if 1
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#define FXTM_ERROR(x)                   (x>>3)
#define FXTM_ERROR_FLASH                (1<<3)
#define FXTM_ERROR_GPS                  (1<<4)
#define FXTM_ERROR_IMU                  (1<<5)
#define FXTM_ERROR_RADIO                (1<<6)
#define FXTM_ERROR_SDCARD               (1<<7)

#define FXTM_FLIGHTSTATUS_MASK          7
#define FXTM_FLIGHTSTATUS(x)            (x&FXTM_FLIGHTSTATUS_MASK)
#define FXTM_FLIGHTSTATUS_LAUNCHPAD     0
#define FXTM_FLIGHTSTATUS_LIFTOFF       1
#define FXTM_FLIGHTSTATUS_BURNOUT       2
#define FXTM_FLIGHTSTATUS_SEPARATION    3
#define FXTM_FLIGHTSTATUS_RECOVERY      4
#define FXTM_FLIGHTSTATUS_TOUCHDOWN     5

#define MAX_SOUND_LEVEL 1024
#define MAX_TEMPERATURE  256

#define GPS_SET(p, tm, LAT, LONG) { \
    tm->g.lat = X; \
    tm->g.lon = Y; \
}

#define IMU_SENSOR_SET(p, tm, X, Y, Z) { \
    tm->p.x = X; \
    tm->p.y = Y; \
    tm->p.z = Z; \
}

#define IMU_SENSOR_GET(p, tm, X, Y, Z) { \
    X = tm->p.x; \
    Y = tm->p.y; \
    Z = tm->p.z; \
}

#define TRAME_VERSION         "BGC0"
#define TRAME_VERSION_SIZE    (sizeof(TRAME_VERSION)- 1)

/* Set Paris as reference for gpsdelta_t */ 
#define GPS_REF_LAT      488252593
#define GPS_REF_LON      23666452

#if 0
typedef short myaccum myfloat;
#elif 0
typedef _Float16 myfloat;
#else
typedef float myfloat;
#endif

#if 0
typedef myfloat imuraw_t;
#else
typedef int16_t imuraw_t;
#endif

#if 0
typedef myfloat gpsraw_t;
typedef myfloat gpsdelta_t;
#else
typedef int32_t gpsraw_t;
typedef int16_t gpsdelta_t;
#endif

typedef struct {
    imuraw_t x;
    imuraw_t y;
    imuraw_t z;
} PACKED imu_sensor_t;

typedef struct {
    gpsdelta_t lat;
    gpsdelta_t lon;
} PACKED gps_t;

typedef struct {
    char          magic[4];
    uint16_t      id;

    uint16_t      pressure;
    uint16_t      diffpressure;

    int8_t        temperature;
    uint8_t       humidity;
#if 0
    uint8_t       soundLevel;
#endif
    uint8_t       battLevel;
    uint8_t       flightStatus;

    gps_t         gps;

    imu_sensor_t  accel;
    imu_sensor_t  accel2;
    imu_sensor_t  gyro;
    imu_sensor_t  magn;

} PACKED fxtm_data_t;

#if 0
enum fxtm_buff_status_e {
    FXTM_NOTINIT,
    FXTM_ACQUIRING,
    FXTM_LOGING,
    FXTM_SENDING
};

typedef struct {
    fxtm_data_t* tm_data;
    fxtm_buff_status_e status;
    bool locked;
} fxtm_buffer_t;

fxtm_buffer_t fxtmbuff[3];

int head = 0;
#define NEXT() {head++; if(head==3) head = 0; }
#endif

typedef struct {
    uint32_t     timestamp;
    uint8_t      padding[64 - sizeof(fxtm_data_t)
                            - sizeof(uint32_t)];
} PACKED fxtm_txheader_t;

typedef struct {
    uint32_t    timestamp;
    int16_t     rssi;
    int16_t     snr;
    int16_t     frequencyError;
} PACKED fxtm_rxfooter_t;

typedef struct {
    fxtm_txheader_t  txh;
    fxtm_data_t      data;
    fxtm_rxfooter_t  rxf;
    uint8_t          padding[512 - sizeof(fxtm_data_t)
                                 - sizeof(fxtm_txheader_t)
                                 - sizeof(fxtm_rxfooter_t)];
} PACKED fxtm_block_t;

//static_assert(sizeof(fxtm_block_t) == 512, "fxtm_block_t must be 512 bytes");
void fxtm_reset(uint32_t ts);
void fxtm_gendata(void);
void fxtm_setsoundlvl(unsigned int level);
void fxtm_setimu(imuraw_t a[], imuraw_t m[], imuraw_t g[]);
void fxtm_setimu2(imuraw_t a[]);
void fxtm_settemperature(int8_t temperature);
void fxtm_setpressure(uint16_t pressure);
void fxtm_setdiffpressure(uint16_t diffpressure);
void fxtm_sethumidity(uint8_t humidity);
void fxtm_setflightstatus(uint8_t flightStatus);
void fxtm_setgps(gpsraw_t latitude, gpsraw_t longitude);
void fxtm_seterror(uint8_t error);

fxtm_data_t*     fxtm_getdata(void);
fxtm_block_t*    fxtm_getblock(void);
fxtm_rxfooter_t* fxtm_getrxfooter(void);
fxtm_txheader_t* fxtm_gettxheader(void);

size_t fxtm_getblocksize(void);
size_t fxtm_getdatasize(void);
size_t fxtm_gettxheadersize(void);
size_t fxtm_getfloatsize(void);
size_t fxtm_getrxfootersize(void);
size_t fxtm_getrxdatasize(void);
size_t fxtm_gettxdatasize(void);

size_t   fxtm_dumprxfooter(fxtm_rxfooter_t * rxf, char* buf, size_t bufsize);
size_t   fxtm_dumptxheader(fxtm_txheader_t* txh, char* buf, size_t bufsize);
size_t   fxtm_dumpdata(fxtm_data_t* tm, char* buf, size_t bufsize);
size_t   fxtm_dumprxdata(uint8_t* data, char* buf, size_t bufsize);
uint16_t fxtm_check(fxtm_data_t* tm, uint16_t* plastid, uint32_t* plastts);
size_t   fxtm_tojson(uint8_t* data, char* buf, size_t bufsize);

void fxtm_getimu(fxtm_data_t* tm, imuraw_t* imu);
void fxtm_getgps(fxtm_data_t* tm, gpsdelta_t* pLatitude, gpsdelta_t* pLongitude);
void fxtm_getpressure(fxtm_data_t* tm, uint16_t* ppressure);
void fxtm_gettxts(fxtm_block_t* data, uint32_t* pts);
void fxtm_getrxts(fxtm_block_t* data, uint32_t* pts);
void fxtm_getid(fxtm_data_t* tm, uint16_t* pid);
void fxtm_getsoundlvl(fxtm_data_t* tm, uint8_t* psndlvl);
void fxtm_gettemperature(fxtm_data_t* tm, int8_t* ptemp);
void fxtm_gethumidity(fxtm_data_t* tm, uint8_t* phumidity);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //define _TRAME_H
