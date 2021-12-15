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

#include <fusexutil.h>
#include <Adafruit_GPS.h>
#include "trame.h"
#include "init.h"

#if 0
#define GPS_DEBUG 1
#endif

#if 0
#define GPS_DEBUG2 1
#endif

#define RETRYMAX 22000

Adafruit_GPS GPS(&GPSdevice);

SIGNAL(TIMER0_COMPA_vect) {
    GPS.read();
}

static void useInterrupt() {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}

void getGPSDateTime(char* str)
{
    sprintf(str,"%d-%d-%d-%dh%dm%d", GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds);
}

bool isGPSFixed()
{
  return !!GPS.fix;
}

void debugGPS()
{
    TRACE("\nTime: %d:%d:%d\r\n", GPS.hour, GPS.minute, GPS.seconds, GPS.milliseconds);
    TRACE("Date: %d/%d/20%d\r\n", GPS.day, GPS.month, GPS.year);
    TRACE("Satellites: %d Fix: %d Quality: %d\r\n", GPS.satellites, (int)GPS.fix, (int)GPS.fixquality);
    if (GPS.fix) {
	PRINT("Location: ");
	PRINT(GPS.latitude, 4); PRINT(GPS.lat);
	PRINT(", ");
	PRINT(GPS.longitude, 4); PRINTLN(GPS.lon);
	PRINT("Location (in degrees, works with Google Maps): ");
	PRINT(GPS.latitudeDegrees, 5);
	PRINT(", "); 
	PRINTLN(GPS.longitudeDegrees, 5);
	TRACE("Speed (knots): %d.%d\r\n", (int)GPS.speed, (int)(GPS.speed*100)%100);
	PRINT("Angle: "); PRINTLN(GPS.angle);
	PRINT("Altitude: "); PRINTLN(GPS.altitude);
	PRINT("Satellites: "); PRINTLN((int)GPS.satellites);
    }
}

void receiveGPS()
{
    if (GPS.newNMEAreceived()) {
#ifdef GPS_DEBUG2
	PRINTLN(GPS.lastNMEA());
#endif
	if (!GPS.parse(GPS.lastNMEA()))
	    return;
    }
    if (GPS.fix) {
	fxtm_setgps(GPS.latitudeDegrees, GPS.longitudeDegrees);
    }

#ifdef GPS_DEBUG
    debugGPS();
#endif
}

void setupGps()
{
  GPS.begin(GPSSERIALBAUD);
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt();

  delay(1000);
  // Ask for firmware version
  GPSdevice.println(PMTK_Q_RELEASE);
  //TODO : check something before mark init done

  // WARNING !!! NO delay() should be called in the next loop from here !
  int retry = 0;
  while(retry++ < RETRYMAX) {
      receiveGPS();
      if(GPS.fix){
	  debugGPS();
	  break;
      }
      TTRACE("....... synchro failed! retry\r\n");
  }
  //
  if(!GPS.fix){
      TTRACE("init Failed ! retry later.\r\n");
      Init_SetFailed();
  } else
      TTRACE("init Done: retry:%d\r\n",retry);
}

void loopGps()
{
    receiveGPS();	
}
