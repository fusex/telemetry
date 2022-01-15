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

#define MAX_SENSORS 2

// IT SHOULD 52

typedef struct {
    char     magic[4] = {'B','G','C','0'};
    uint16_t id;
    uint16_t rawpressure[MAX_SENSORS];
    int8_t   temperature[MAX_SENSORS];

    int8_t   humidity;
    uint8_t  soundlvl;

    int32_t  gpslt; 
    int32_t  gpslg;

    uint8_t  highgyrx:HBITFILED;
    uint8_t  highgyry:HBITFILED;
    uint8_t  highgyrz:HBITFILED;

    uint8_t  highaccelx:HBITFILED;
    uint8_t  highaccely:HBITFILED;
    uint8_t  highaccelz:HBITFILED;

    uint8_t  highmagnx:HBITFILED;
    uint8_t  highmagny:HBITFILED;
    uint8_t  highmagnz:HBITFILED;

    uint8_t  signgyrx:SBITFILED;
    uint8_t  signgyry:SBITFILED;
    uint8_t  signgyrz:SBITFILED;

    uint8_t  signaccelx:SBITFILED;
    uint8_t  signaccely:SBITFILED;
    uint8_t  signaccelz:SBITFILED;

    uint8_t  signmagnx:SBITFILED;
    uint8_t  signmagny:SBITFILED;
    uint8_t  signmagnz:SBITFILED;

    uint8_t  highgyr2x:HBITFILED;
    uint8_t  highgyr2y:HBITFILED;
    uint8_t  highgyr2z:HBITFILED;

    uint8_t  highaccel2x:HBITFILED;
    uint8_t  highaccel2y:HBITFILED;
    uint8_t  highaccel2z:HBITFILED;

    uint8_t  signgyr2x:SBITFILED;
    uint8_t  signgyr2y:SBITFILED;
    uint8_t  signgyr2z:SBITFILED;

    uint8_t  signaccel2x:SBITFILED;
    uint8_t  signaccel2y:SBITFILED;
    uint8_t  signaccel2z:SBITFILED;
} __attribute__((packed)) fxtm_data_t;

#define MASK0 0x0000ffff
#define MASK1 0x000f0000

#define SET(tm, comp, val) { \
	int32_t tval = val; \
        if(val<0){ \
            tm->sign##comp = 1; \
            tval = -val; \
        } \
        tm->comp       = tval&MASK0; \
        tm->high##comp = (tval&MASK1)>>16; \
    }

#define GET(tm, comp, val) { \
	val = tm->comp + (((uint32_t)tm->high##comp)<<16); \
	if(tm->sign##comp) \
	val = -val; \
    }

#define SETT(p, tm, X, Y, Z) { \
    SET(tm, p##x, X); \
    SET(tm, p##y, Y); \
    SET(tm, p##z, Z); \
}

#define GETT(p, tm, X, Y, Z) { \
    GET(tm, p##x, X); \
    GET(tm, p##y, Y); \
    GET(tm, p##z, Z); \
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

#if 0
// Number of data records in a block.
//const uint16_t DATA_DIM = (512 - 4)/sizeof(fxtm_data_t);
#define DATA_DIM ((512 - 4)/sizeof(fxtm_data_t))

//Compute fill so block size is 512 bytes.  FILL_DIM may be zero.
//const uint16_t FILL_DIM = 512 - 4 - DATA_DIM*sizeof(fxtm_data_t);
#define FILL_DIM (512 - 4 - DATA_DIM*sizeof(fxtm_data_t))
#else
#define FILL_DIM (512 - sizeof(fxtm_data_t))
#endif

typedef struct {
  fxtm_data_t  data;
  uint8_t      fill[FILL_DIM];
} fxtm_block_t;

#if 0
#define PRESSURE_AT_SEALEVEL (101325)
#define GET_RAWPRESSURE(p) (uint16_t)(PRESSURE_AT_SEALEVEL - p)
#define GET_PRESSURE(raw)  (uint32_t)(PRESSURE_AT_SEALEVEL - raw)
#else
#define MAX_PRESSURE_AT_SEALEVEL (10300)
#define GET_RAWPRESSURE(p) (uint16_t)(MAX_PRESSURE_AT_SEALEVEL - p)
#define GET_PRESSURE(raw)  (uint32_t)(MAX_PRESSURE_AT_SEALEVEL - raw)
#endif

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
