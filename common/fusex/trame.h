#ifndef _TRAME_H
#define _TRAME_H

#include <stdint.h>
#include <stdbool.h>

#define FXTM_FLIGHTSTATUS_LAUNCHPAD     0
#define FXTM_FLIGHTSTATUS_LIFTOFF       1
#define FXTM_FLIGHTSTATUS_BURNOUT       2
#define FXTM_FLIGHTSTATUS_SEPARATION    3
#define FXTM_FLIGHTSTATUS_RECOVERY      4
#define FXTM_FLIGHTSTATUS_TOUCHDOWN     5

#define MAX_SOUND_LEVEL 1024
#define MAX_TEMPERATURE  256

#define IMU_SENSOR_SET(p, tm, X, Y, Z) { \
    tm->p.x = X; \
    tm->p.y = Y; \
    tm->p.z = Z; \
}

#define IMU_SENSOR_GET(p, tm, X, Y, Z) { \
    X = tm->p.x;            \
    Y = tm->p.y;            \
    Z = tm->p.z;            \
}

#if 0
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#ifdef __cplusplus
#define myaccum 
extern "C"
{
#else
#define myaccum _Accum
#endif /* __cplusplus */

typedef struct {
    short myaccum x;
    short myaccum y;
    short myaccum z;
} imu_sensor_t;

#define TRAME_VERSION         "BGC0"
#define TRAME_VERSION_SIZE    (sizeof(TRAME_VERSION)- 1)
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

    short myaccum gpsLt;
    short myaccum gpsLg;

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
} fxtm_txheader_t;

typedef struct {
    uint32_t    timestamp;
    int16_t     rssi;
    int16_t     snr;
    int16_t     frequencyError;
} fxtm_rxfooter_t;

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
void fxtm_setimu(float a[], float m[], float g[]);
void fxtm_setimu2(float a[]);
void fxtm_settemperature(int8_t temperature);
void fxtm_setpressure(uint16_t pressure);
void fxtm_setdiffpressure(uint16_t diffpressure);
void fxtm_sethumidity(uint8_t humidity);
void fxtm_setflightstatus(uint8_t flightStatus);
void fxtm_setgps(float latitude, float longitude);

fxtm_data_t*     fxtm_getdata(void);
fxtm_block_t*    fxtm_getblock(void);
fxtm_rxfooter_t* fxtm_getrxfooter(void);
fxtm_txheader_t* fxtm_gettxheader(void);

size_t fxtm_getblocksize(void);
size_t fxtm_getdatasize(void);
size_t fxtm_gettxheadersize(void);
size_t fxtm_getrxfootersize(void);
size_t fxtm_getrxdatasize(void);
size_t fxtm_gettxdatasize(void);

size_t   fxtm_dumprxfooter(fxtm_rxfooter_t * rxf, char* buf, size_t bufsize);
size_t   fxtm_dumptxheader(fxtm_txheader_t* txh, char* buf, size_t bufsize);
size_t   fxtm_dumpdata(fxtm_data_t* tm, char* buf, size_t bufsize);
size_t   fxtm_dumprxdata(uint8_t* data, char* buf, size_t bufsize);
uint16_t fxtm_check(fxtm_data_t* tm, uint16_t* plastid, uint32_t* plastts);
size_t   fxtm_tojson(uint8_t* data, char* buf, size_t bufsize);

void fxtm_getimu(fxtm_data_t* tm, float* imu);
void fxtm_getgps(fxtm_data_t* tm, float* gps);
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
