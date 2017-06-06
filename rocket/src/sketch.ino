#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"

#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"

block_t  fxtmblock;
uint16_t idCounter = 0;

void setup()
{
    setupCommon();
    setupRadio();
    setupImu();
    setupGps();
    setupPropellant();
    setupSdcard();
}

static void acquire()
{
    fxtmblock.data.timestamp = millis();
    fxtmblock.data.id        = idCounter++; 

    loopPropellant();
    loopImu();
    loopGps();
#if 0 
    fxtmdump(&fxtmblock.data); 
#endif
}

static void log()
{
    loopSdcard();
}

static void send()
{
    loopRadio();
}

void loop()
{
    uint32_t time = micros();
    acquire();
    log();
    send();
#if 0
    TTRACE("id:%u loop in :%ld\r\n", fxtmblock.data.id, micros()-time);
    delay(100);
#endif
}
