#ifndef _REAL_H
#define _REAL_H

#include <stdint.h>
#include <stdbool.h>

#include "trame.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef float imureal_t;

typedef struct {
    imureal_t x;
    imureal_t y;
    imureal_t z;
    uint32_t  r;
} imureal_sensor_t;

typedef float gpsreal_t;

typedef struct {
    gpsreal_t lat;
    gpsreal_t lon;
} gps_position_t;

typedef struct {
    float    speedx;
    float    speedy;
} gspeed_t;

typedef struct {
    float    qx;
    float    qy;
    float    qz;
    float    qw;
} quaternion_t;

typedef struct {
    float    px;
    float    py;
    float    pz;
} position_t;

typedef struct {
    int16_t  rssi;
    int16_t  snr;
    int16_t  frequencyError;
    float    frequency;
} radio_t;

typedef struct {
    int8_t            temperature;
    uint8_t           humidity;
    uint16_t          pressure;
} atmos_t;

typedef struct {
    uint32_t          id;
    uint32_t          timestamp;

    uint16_t          diffpressure;
    atmos_t           atmos;

#if 0
    uint8_t           soundLevel;
#endif

    uint8_t           battLevel;
    uint8_t           flightStatus;
    uint8_t           errors;

    float             altitude;
    gps_position_t    gps;

    imureal_sensor_t  accel;
    imureal_sensor_t  gyro;
    imureal_sensor_t  magn;
    imureal_sensor_t  accel2;
    gspeed_t          groundspeed;
    quaternion_t      orientation;
    position_t        position;

    radio_t           radio;
} fxreal_data_t;


void fxreal_new(fxtm_data_t* tm);
void fxreal_setaccel(fxtm_data_t* tm);
void fxreal_setgps(fxtm_data_t* tm);
void fxreal_setaccel2(fxtm_data_t* tm);
void fxreal_setgyro(fxtm_data_t* tm);
void fxreal_setmagn(fxtm_data_t* tm);
void fxreal_setatmos(fxtm_data_t* tm);
void fxreal_setradio(fxtm_data_t* tm);
void fxreal_setpitot(fxtm_data_t* tm);
void fxreal_setflightstatus(fxtm_data_t* tm);
void fxreal_seterror(fxtm_data_t* tm);
void fxreal_setbattlevel(fxtm_data_t* tm);
void fxreal_finish();
void fxreal_new(fxtm_data_t* tm);
size_t fxreal_tojson(uint8_t* data, char* buf, size_t bufsize);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //define _REAL_H
