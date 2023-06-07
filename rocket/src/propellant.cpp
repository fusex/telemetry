#define TAG "SND"

#include <Arduino.h>
#include <fusexutil.h>
#include <trame.h>

#include "BGC_Pinout.h"

const static int thresholdvalue=490; //The threshold

#if 0
#define SOUNDSENSOR_DEBUG
#endif

void setupPropellant ()
{
    module_add(TAG);
    TTRACE("init Done\n\r");
    module_setup(TAG, FXTM_SUCCESS);
}

void loopPropellant ()
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
