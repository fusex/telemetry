#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"

#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"

block_t fxtmblock;

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
    loopPropellant();
    loopImu();
    loopGps();
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
    TTRACE("loop in :%ld\r\n", micros()-time);
}
