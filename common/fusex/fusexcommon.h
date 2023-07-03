#ifndef _FUSEXCOMMON_H
#define _FUSEXCOMMON_H

#define ACCEL_RANGE            (16)
#define ACCEL2_RANGE           (2)
#define GYRO_RANGE             (500)

//TODO to be tuned!
#if 1
#define RADIO_FREQ             (869.4)
#else
#define RADIO_FREQ             (434.0)
#endif

/* Set Paris as reference for gpsdelta_t */ 
#define GPS_REF_LAT            (488252593)
#define GPS_REF_LON            (23666452)

/* Reference to calculate Altitude */
#define PRESSURE_SEALEVEL_REF  (1013.25)

#endif // _FUSEXCOMMON_H
