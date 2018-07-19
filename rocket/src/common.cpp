/*
 * =====================================================================================
 *
 *       Filename:  common.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:44:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#define TAG "CMN"

#include <fusexutil.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <stdbool.h>

#include "trame.h"
#include "pinout.h"
#include "rtc.h"

#define MAXFATAL  4
#define MAXRESET 10

#if 1
#define COMMON_DEBUG 
#endif

bool initfailed = false;
uint8_t semifatalcount = 0;
bool skip = true;

void setupCommon()
{
    MCUSR = 0; // clear out any flags of prior resets.
    randomSeed(analogRead(0));
    DEBUGdevice.begin(DEBUGSERIALBAUD);
    pinMode(LEDFATAL, OUTPUT);
    digitalWrite(LEDFATAL, HIGH);

#ifndef CONFIG_PROD
    while (!DEBUGdevice) ; // Wait for DEBUG serial port to be available
#endif
    TTRACE("#######################\r\n");
    TTRACE("Rocket init Start\r\n");
}

void setupFinishCommon()
{
    uint16_t rid = getResetID();
    if(rid>MAXRESET)
	setResetResetID();

    rid = getResetID();

    if(!initfailed) {
	digitalWrite(LEDFATAL, LOW);
	TTRACE("Rocket init Done\r\n");
     } else {
	TTRACE("Rocket init failed !\r\n");
	for(int i=0;i<100;i++) {
	   digitalWrite(LEDFATAL, LOW);
	   delay(100);
	   digitalWrite(LEDFATAL, HIGH);
	   delay(100);
	}
#ifdef COMMON_DEBUG
	TTRACE("rid :%d and semifatalcount:%d!\r\n",rid,semifatalcount);
#endif

	if(semifatalcount==MAXFATAL && rid<MAXRESET) {
	    setResetID();
	    TTRACE("FATAL init detected! reset !\r\n");
	    wdt_enable(WDTO_15MS); //turn on the WatchDog and don't stroke it.
	    while(1);
	}
     }
     TTRACE("#######################\r\n");
}

void setupSetFailed()
{
    initfailed = true;
}

void setupSetSemiFatal()
{
    initfailed = true;
    semifatalcount++;
}

void setupSetFatal()
{
    initfailed = true;
    semifatalcount = MAXFATAL;
    setupFinishCommon();
}
