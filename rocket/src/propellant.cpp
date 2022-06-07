/*
 * =====================================================================================
 *
 *       Filename:  propellant.cpp
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

#define TAG "SND"

#include <Arduino.h>
#include <fusexutil.h>
#include <fusexconfig.h>

#include "BGC_Pinout.h"
#include "trame.h"

const static int thresholdvalue=490; //The threshold

#if 0
#define SOUNDSENSOR_DEBUG
#endif

void setupPropellant()
{
   TTRACE("init Done\n\r");
}

void loopPropellant()
{
    int sensorValue = analogRead(BGC_SOUND_SENSOR);

#ifdef SOUNDSENSOR_DEBUG
    if(sensorValue>thresholdvalue)
	TTRACE("Propellant detect:%d\r\n", sensorValue);
    else
	TRACE("Propellant detect:%d\r\n", sensorValue);
#endif

    fxtm_setsoundlvl(sensorValue);
}
