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

#define TAG "COMMON"

#include <fusexutil.h>
#include <Arduino.h>

#include "trame.h"
#include "pinout.h"

bool initfailed = false;

void setupCommon()
{
    randomSeed(analogRead(0));
    DEBUGdevice.begin(DEBUGSERIALBAUD);
    pinMode(LEDFATAL, OUTPUT);

#ifndef CONFIG_PROD
    while (!DEBUGdevice) ; // Wait for DEBUG serial port to be available
#endif

    TTRACE("#######################\r\n");
    TTRACE("Rocket init Start\r\n");
}

void setupFinishCommon()
{
    if(!initfailed)
	TTRACE("Rocket init Done\r\n");
     else {
	TTRACE("Rocket init failed ! FATAL !\r\n");
	bool toggle = LOW;
	while(1) {
	   digitalWrite(LEDFATAL, toggle++);
	   delay(1);
	}
     }
     TTRACE("#######################\r\n");
}

void setupSetFailed()
{
    initfailed = true;
}
