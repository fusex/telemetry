/*
* =====================================================================================
*
*       Filename:  trame.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  05/02/2017 01:09:39 PM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@derbsellicon.com
*        Company:  Derb.io 
*
* =====================================================================================
*/

#ifndef _TRAME_H
#define _TRAME_H

#include <stdint.h>

#define HBITFILED 4
#define SBITFILED 1

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} imu_sensor_t;

#define MAX_SENSORS 2

// IT SHOULD BE arround 52 Bytes
typedef struct {
    char          magic[4] = {'B','G','C','0'};
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

    int32_t       gpsLt; 
    int32_t       gpsLg;

    imu_sensor_t  accel;
    imu_sensor_t  accel2;
    imu_sensor_t  gyro;
    imu_sensor_t  magn;

} __attribute__((packed)) fxtm_data_t;

#define MASK0 0x0000ffff
#define MASK1 0x000f0000

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

#define FILL_DIM (512 - 4 - sizeof(fxtm_data_t))
typedef struct {
  fxtm_data_t  data;
  uint32_t     timestamp; 
  uint8_t      fill[FILL_DIM];
} fxtm_block_t;


#define FXTM_FLIGHTSTATUS_LAUNCHPAD     0
#define FXTM_FLIGHTSTATUS_LIFTOFF       1
#define FXTM_FLIGHTSTATUS_BURNOUT       2
#define FXTM_FLIGHTSTATUS_SEPARATION    3
#define FXTM_FLIGHTSTATUS_RECOVERY      4
#define FXTM_FLIGHTSTATUS_TOUCHDOWN     5

void fxtm_reset(void);
void fxtm_gendata(void);
void fxtm_setsoundlvl(unsigned int level);
void fxtm_setimu(float a[], float m[], float g[]);
void fxtm_setimu2(float a[]);
void fxtm_settemperature(float temperature);
void fxtm_setpressure(float pressure);
void fxtm_setdiffpressure(uint16_t diffpressure);
void fxtm_sethumidity(float pressure);
void fxtm_setflightstatus(uint8_t flightStatus);
void fxtm_setgps(float latitude, float longitude);

fxtm_data_t*  fxtm_getdata();
fxtm_block_t* fxtm_getblock();

size_t fxtm_getblocksize();
size_t fxtm_getdatasize();
void   fxtm_dump(bool isConsole);
int    fxtm_check(fxtm_data_t* tm);

void fxtm_getimu(fxtm_data_t* tm, float* imu);
void fxtm_getgps(fxtm_data_t* tm, float* gps);
void fxtm_getpressure(fxtm_data_t* tm, int32_t* ppressure);
void fxtm_getts(fxtm_data_t* tm, uint32_t* pts);
void fxtm_getid(fxtm_data_t* tm, uint16_t* pid);
void fxtm_getsoundlvl(fxtm_data_t* tm, uint8_t* psndlvl);
void fxtm_gettemperature(fxtm_data_t* tm, int8_t* ptemp);
void fxtm_gethumidity(fxtm_data_t* tm, int8_t* phumidity);

#define GPSFACTORPOW   "6" 
#define GPSFACTOR 1000000 
#define IMUFACTOR     100 

#define MAX_SOUND_LEVEL 1024
#define MAX_TEMPERATURE  256

#endif //define _TRAME_H
