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
#include "BGC_Pinout.h"
#include "rtc.h"
#include "version.h"

#define MAXFATAL  4
#define MAXRESET 10

#if 0
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

    pinMode(BGC_LED4_FERROR, OUTPUT);
    digitalWrite(BGC_LED4_FERROR, HIGH);

#ifndef CONFIG_PROD
    while (!DEBUGdevice) ; // Wait for DEBUG serial port to be available
#endif
    TTRACE("#######################\r\n");
    TTRACE("Rocket init Start\r\n");
    TTRACE("    Version: %s\r\n", VERSION);
}

void Init_ResetBoard()
{
    //turn on the WatchDog and don't stroke it.
    wdt_enable(WDTO_15MS);
    while(1);
}

void Init_Finish()
{
    uint16_t rid = RTC_GetResetID();

    if(rid>MAXRESET)
	    RTC_ResetResetID();

    rid = RTC_GetResetID();

    if(!initfailed) {
	    digitalWrite(BGC_LED4_FERROR, LOW);
	    TTRACE("Rocket init Done\r\n");
     } else {
	    TTRACE("Rocket init failed !\r\n");
	    for(int i=0;i<100;i++) {
	        digitalWrite(BGC_LED4_FERROR, LOW);
	        delay(100);
	        digitalWrite(BGC_LED4_FERROR, HIGH);
	        delay(100);
	    }

	    TTRACE("rid :%d and semifatalcount:%d!\r\n",rid,semifatalcount);

	    if(semifatalcount==MAXFATAL && rid<MAXRESET) {
	        RTC_SetResetID();
	        TTRACE("FATAL init detected! reset !\r\n");
		Init_ResetBoard();
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
