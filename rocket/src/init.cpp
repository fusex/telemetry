/*
 * =====================================================================================
 *
 *       Filename:  init.cpp
 *
 *    Description:  Board low level initialization
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:44:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@elqotbi.com
 *
 * =====================================================================================
 */

#define TAG "INI"

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
#define STP_DEBUG 
#endif

static bool initfailed = false;
static uint8_t semifatalcount = 0;
static bool skip = true;

void setupInit()
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

void Init_Finish()
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

	    TTRACE("rid :%d and semifatalcount:%d!\r\n",rid,semifatalcount);

	    if(semifatalcount==MAXFATAL && rid<MAXRESET) {
	        setResetID();
	        TTRACE("FATAL init detected! reset !\r\n");
	        wdt_enable(WDTO_15MS); //turn on the WatchDog and don't stroke it.
	        while(1);
	    }
    }
    TTRACE("#######################\r\n");
}

void Init_SetFailed()
{
    initfailed = true;
}

void Init_SetSemiFatal()
{
    initfailed = true;
    semifatalcount++;
}

void Init_SetFatal()
{
    initfailed = true;
    semifatalcount = MAXFATAL;
    Init_Finish();
}
