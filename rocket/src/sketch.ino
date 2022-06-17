#define TAG "MAIN"

#include "init.h"
#include "imu.h"
#include "atmos.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"
#include "rtc.h"
#include "prof.h"

#include <SimpleTimer.h>
#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"
#include "debug.h"
#include "shell.h"

SimpleTimer scheduler; 

static void acquire_fake()
{
    fxtm_reset();
    fxtm_gendata();
}

static void acquire()
{
    fxtm_reset();
    loopRTC();
    loopPropellant();
    loopAtmos();
    loopIMU();
    //loopGps();
    //loopFlightStatus();
}


static void log()
{
    //loopSdcard();
    //fxtm_dump(false);
}

static void send()
{
    loopRadio();
}

void subLoop()
{
    prof_start();

#if 1
    acquire();
#else
    acquire_fake();
#endif
    log();
    send();

    prof_report();
}

#define BGC_ACQ_PERIOD 1000 // in millis on each second.

void setup()
{
    setupInit();
    setupShell();
    setupRTC();
    setupPropellant();
    setupAtmos();
    setupIMU();
    setupRadio();
    //setupGps();
    //setupSdcard();
    Init_Finish();
    TTRACE("#########################\n\r");
	TTRACE("Start transfer fxtm_data size:%d\n\r", fxtm_getdatasize());
    scheduler.setTimer(BGC_ACQ_PERIOD, subLoop, scheduler.RUN_FOREVER);
}

void loop()
{
    loopShell();
    if (!execIsPaused()) {
        scheduler.run();
    }
}
