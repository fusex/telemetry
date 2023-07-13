#ifndef _FUSEXCOMMON_H
#define _FUSEXCOMMON_H

#include <stdio.h>

#define ACCEL_RANGE            (16)
#define ACCEL2_RANGE           (2)
#define GYRO_RANGE             (500)

//TODO to be tuned!
#if 1
#define RADIO_FREQ             (869.4)
#else
#define RADIO_FREQ             (434.0)
#endif


//TODO read that from sdcard!
#if 0
/* Set Paris as reference for gpsdelta_t */ 
#define GPS_REF_LAT            (488252593)
#define GPS_REF_LON            (23666452)
#warning PARIS is used as center of gps telem
#else
#define GPS_REF_LAT            (43.2085793)
#define GPS_REF_LON            (-0.06237763)
#warning CAMPS of GER is used! to be confirmed!
#endif

#if 1
/* about 4.7 km of radius */
#define GPS_PRECISION_FACTOR    (10)
#elif 0
/* about 470 meters of radius */
#define GPS_PRECISION_FACTOR    (1)
#elif 0
/* about 47 km of radius */
#define GPS_PRECISION_FACTOR    (100)
#endif

/* Reference to calculate Altitude */
#define PRESSURE_SEALEVEL_REF  (1013.25)

#define STRINGIFY(f, ...) do { \
    if (remaining>0) { \
        wrote = snprintf(buf+totalwrote, remaining, f,  ##__VA_ARGS__); \
        totalwrote += wrote; \
        remaining -= wrote;  \
    } \
} while(0);

#endif // _FUSEXCOMMON_H
