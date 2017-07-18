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
#define DEBUGdevice Serial1
#define GPSdevice   Serial
#define IMU_CALIBRATION 1
#define CONFIG_FATAL 1
#else
#if defined(__AVR_ATmega2560__)
#define _IS_ROCKET 1
//#pragma message ("this is the Mega board")
#define DEBUGdevice Serial1
#define GPSdevice   Serial
#elif defined(__AVR_ATmega328P__)
#define _IS_BASE 1
//#pragma message ("this is the UNO board")
#define PCdevice    Serial
#define DEBUGdevice SWSerial
#elif defined(__i386__) || defined( __x86_64__)
#define _IS_PC 1 
#else
#error unknown board
#endif
#define IMU_CALIBRATION 0
#define CONFIG_FATAL 0
#endif

#endif // _FUSEXCONFIG_H
