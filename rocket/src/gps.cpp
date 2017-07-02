/*
 * =====================================================================================
 *
 *       Filename:  gps.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:23:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */


#if 0
#define TAG "GPS"

#include <NMEAGPS.h>
#include <fusexutil.h>

#include "trame.h"

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

uint8_t ms = 0;

SIGNAL(TIMER0_COMPA_vect)
{
#if 0
    if(ms++<10) return;
    ms = 0;
#endif
    while (GPSdevice.available()) {
	char inChar = (char)GPSdevice.read();
	gps.handle(inChar);
    }
}

void setupGps()
{
    GPSdevice.begin(9600);
    while (!GPSdevice){
	TTRACE("init failed ! Retrying !\n\r");
	delay(2000);
    }

    TTRACE("init Done\r\n");

    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}

uint32_t time = 0;

#include <Streamers.h>

void loopGps()
{
#if 0
    if (gps.overrun()) {
	gps.overrun( false );
	TRACE("DATA OVERRUN\r\n");
    }
#endif

    if(!gps.available()) {
	return;
    }

    fix = gps.read();

#if 1
    trace_all(DEBUGdevice, gps, gps.read());
    TRACE("\r\n");

    if (fix.valid.satellites){
	TTRACE("Satellites: %d/%d)\r\n", fix.satellites, gps.sat_count);
	for (uint8_t i=0; i < gps.sat_count; i++) {
	    TRACE("\tid: %d\tsnr: %d\ttracked: %s\r\n",
		  gps.satellites[i].id,
                  gps.satellites[i].snr,
		  gps.satellites[i].tracked?"Yes":"NO");
	}
    }
#endif

    if (fix.valid.location) {
#if 1
	#define TOSTR0(x) dtostrf(x,3,6,tmp0)
	#define TOSTR1(x) dtostrf(x,3,6,tmp1)
	#define TOSTR2(x) dtostrf(x,3,6,tmp2)
	char tmp0[16];
	char tmp1[16];
	char tmp2[16];
	TTRACE("Location: %s, %s alt:%s\r\n",TOSTR0(fix.latitude()), TOSTR1(fix.longitude()), TOSTR2(fix.valid.altitude));
#endif
	fxtm_setgps(fix.latitude(), fix.longitude());
    }
}
#endif
