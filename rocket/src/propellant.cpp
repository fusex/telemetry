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

#include <Arduino.h>
#include <fusexutil.h>
#include <fusexconfig.h>

const int thresholdvalue=490; //The threshold

void setupPropellant()
{
#ifndef CONFIG_PROPELLANT
    return;
#endif
   TRACE("Init Propellant sound detector\n\r");
}

void loopPropellant()
{
#ifndef CONFIG_PROPELLANT
    return;
#endif
    int sensorValue = analogRead(A13); //use A0 to read the electrical signal
    if(sensorValue>thresholdvalue)
    {
	TRACE("Propellant detect:%d\r\n", sensorValue);
    }
    delay(200);
}

