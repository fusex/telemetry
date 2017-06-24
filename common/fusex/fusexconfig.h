#ifndef _FUSEXCONFIG_H
#define _FUSEXCONFIG_H

#if 0
#define CONFIG_PROD
#endif

#if 0
#define CONFIG_DEBUG
#endif

#if 0
#define CONFIG_CHECKCRC
#endif

#if 1
#define CONFIG_DONT_WAIT
#endif

#define CONFIG_PACKETNUMBER 1

#define CONFIG_SDCARD
#define CONFIG_RADIO
#define CONFIG_IMU
#if 0
#define CONFIG_PROPELLANT
#define CONFIG_GPS
#endif

#ifdef CONFIG_SDCARD
#define CONFIG_TEST_SDCARD
#endif

#ifdef CONFIG_RADIO
#define CONFIG_TEST_RADIO
#endif

#ifdef CONFIG_IMU
#define CONFIG_TEST_IMU
#endif

#ifdef CONFIG_PROPELLANT
#define CONFIG_TEST_PROPELLANT
#endif

#ifdef CONFIG_GPS
#define CONFIG_TEST_GPS
#endif

#ifdef CONFIG_PROD
#define DEBUGDevice Serial1
#define GPSdevice   Serial
#else
#define DEBUGDevice Serial
#define GPSdevice   Serial1
#endif

#endif // _FUSEXCONFIG_H
