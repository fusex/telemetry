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

Adafruit_GPS GPS(&GPSdevice);

SIGNAL(TIMER0_COMPA_vect) {
    GPS.read();
}

void useInterrupt() {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}

void setupGps()
{
  GPS.begin(GPSSERIALBAUD);
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  //GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt();

  delay(1000);
  // Ask for firmware version
  GPSdevice.println(PMTK_Q_RELEASE);
}

#define GPS_DEBUG 1
#define GPS_DEBUG2 0

extern int dump;

void loopGps()
{
    if (GPS.newNMEAreceived()) {
#if GPS_DEBUG2
	PRINTLN(GPS.lastNMEA());
#endif
	if (!GPS.parse(GPS.lastNMEA()))
	    return;
    }
    if (GPS.fix) {
	fxtm_setgps(GPS.latitudeDegrees, GPS.longitudeDegrees);
    }

#if GPS_DEBUG
    if(dump) {
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
#endif
}
