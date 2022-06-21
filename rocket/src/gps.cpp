/*
 * =====================================================================================
 *
 *       Filename:  gps2.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2017 16:49:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#define TAG "GPS"

#include <NMEAGPS.h>
#include <NeoHWSerial.h>
#include <fusexutil.h>
#include <BGC_Pinout.h>

#include "trame.h"
#include "init.h"

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( GPS_FIX_LOCATION )
  #error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

#if !defined( NMEAGPS_INTERRUPT_PROCESSING )
  #error You must define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

#define RETRYMAX 60 // Retry 60 secondes.
static NMEAGPS gps; // This parses the GPS characters

// The parser is expecting only GNGLL messages at 5Hz rate.
// Please use ubloxcfg.ino to configure the ublox gps receiver accordingly.

// Complicated macros to perform double indirection inorder to append
// "Neo" to the Serial name ex: BGC_SerialGPS == Serial1  => GPSdevice == NeoSerial1.
#define VARIABLE Neo
#define PASTER(x,y) x ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define NAME(fun) EVALUATOR(VARIABLE, fun)

#define GPSdevice NAME(BGC_SerialGPS)

static void GPSisr( uint8_t c )
{
    gps.handle( c );
}

static int receiveGPS ()
{
    int ret = -1;

    if (gps.available()) {
	gps_fix fix = gps.read();
	if (fix.valid.location) {
	    fxtm_setgps(fix.latitude(), fix.longitude());
	    ret = 0; 
	}
    }

    return ret;
}

void setupGps ()
{
    bool gpsFixed = false;

    GPSdevice.attachInterrupt(GPSisr);
    GPSdevice.begin(GPSSERIALBAUD);

    int retry = 0;
    while (retry++ < RETRYMAX) {
        if (receiveGPS() == 0) {
	    gpsFixed = true;
            break;
	}
	delay(1000);
    }
    //
    if (gpsFixed == false) {
        TTRACE("init Failed ! retry later.\r\n");
        Init_SetFailed();
    } else {
        TTRACE("init Done: retry:%d\r\n", retry);
    }
}

void loopGps()
{
    receiveGPS();
    if (gps.overrun()) {
        gps.overrun( false );
    }
}
