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
    uint8_t  low;
    uint8_t  high:HBITFILED;
    uint8_t  sign:SBITFILED;
} imu_pack_t;

typedef struct {
    imu_pack_t x;
    imu_pack_t y;
    imu_pack_t z;
} imu_sensor_t;

#define MAX_SENSORS 2
// IT SHOULD 52
typedef struct {
    char          magic[4] = {'B','G','C','0'};
    uint16_t      id;

    uint16_t      pressure;
    uint16_t      diffpressure;

    int8_t        temperature;
    int8_t        humidity;
    uint8_t       soundLevel;

    int32_t       gpsLt; 
    int32_t       gpsLg;
    imu_sensor_t  accel;
    imu_sensor_t  gyro;
    imu_sensor_t  magn;
    imu_sensor_t  accel2;
    imu_sensor_t  gyro2;
} __attribute__((packed)) fxtm_data_t;

#define MASK0 0x0000ffff
#define MASK1 0x000f0000

#define IMU_PACK_SET(comp, val) {  \
	int32_t tval = val;            \
    if (val<0) {                   \
        comp.sign = 1;             \
        tval = -val;               \
    }                              \
    comp.low   = tval&MASK0;       \
    comp.high  = (tval&MASK1)>>16; \
}

#define IMU_PACK_GET(comp, val) {                 \
	val = comp.low + (((uint32_t)comp.high)<<16); \
	if (comp.sign)                                \
	    val = -val;                               \
}

#define IMU_SENSOR_SET(p, tm, X, Y, Z) { \
    IMU_PACK_SET(tm->p.x, X); \
    IMU_PACK_SET(tm->p.y, Y); \
    IMU_PACK_SET(tm->p.z, Z); \
}

#define IMU_SENSOR_GET(p, tm, X, Y, Z) { \
    IMU_PACK_GET(tm->p.x, X);            \
    IMU_PACK_GET(tm->p.y, Y);            \
    IMU_PACK_GET(tm->p.z, Z);            \
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

void fxtm_reset(void);
void fxtm_setsoundlvl(unsigned int level);
void fxtm_setimu(float a[], float m[], float g[]);
void fxtm_settemperature(float temperature);
void fxtm_setpressure(float pressure);
void fxtm_setgps(float latitude, float longitude);

fxtm_data_t*  fxtm_getdata();
fxtm_block_t* fxtm_getblock();

size_t fxtm_getblocksize();
size_t fxtm_getdatasize();
void   fxtm_dump(fxtm_data_t*);
int    fxtm_check(fxtm_data_t* tm);

void fxtm_getimu(fxtm_data_t* tm, float* imu);
void fxtm_getgps(fxtm_data_t* tm, float* gps);
void fxtm_getpressure(fxtm_data_t* tm, int32_t* ppressure);
void fxtm_getts(fxtm_data_t* tm, uint32_t* pts);
void fxtm_getid(fxtm_data_t* tm, uint16_t* pid);
void fxtm_getsoundlvl(fxtm_data_t* tm, uint8_t* psndlvl);
void fxtm_gettemperature(fxtm_data_t* tm, int8_t* ptemp);

#define GPSFACTORPOW   "6" 
#define GPSFACTOR 1000000 
#define IMUFACTOR     100 

#define MAX_SOUND_LEVEL 1024
#define MAX_TEMPERATURE  256

#endif //define _TRAME_H