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

#define TAG "GPS"

#include <NMEAGPS.h>
#include <fusexutil.h>

#include "trame.h"

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

extern block_t fxtmblock;

void serialEvent()
{
    char inChar = (char)GPSdevice.read();
    gps.handle(inChar);
}

void setupGps()
{
    GPSdevice.begin(9600);
    while (!GPSdevice);

    TTRACE("init Done\r\n");
}

uint32_t time = 0;

void loopGps()
{
#if 0
    if((millis() - time)<1000)
	return;

    time = millis();
#endif

    fxtm_data_t* fxtmdata = (fxtm_data_t*) &fxtmblock;
#if 0
//while(1) {
   TTRACE("GPSTRY\r\n");
   int co = 0;
retry:
    while(gps.available(GPSdevice)) {
#endif

    if(gps.available()) {
	fix = gps.read();
	TTRACE("GPSIN\r\n");
	if (fix.valid.location) {
	    TTRACE("Location: %6f, %6f alt:%2f\r\n",fix.latitude(), fix.longitude(), fix.valid.altitude);
	    fxtmdata->gpslt = (uint32_t)(fix.latitude()*1000000);
	    fxtmdata->gpslg = (uint32_t)(fix.longitude()*1000000);
	    //break;
	}
    }
#if 0
    if (co++ <10)
    goto retry;
//}
    //TRACE("BREAK\r\n");
#endif
}
