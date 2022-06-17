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

static void acquire()
{
    fxtm_reset();
#if 0
    loopRTC();
    loopPropellant();
    loopAtmos();
    loopGps();
    loopIMU();
    loopFlightStatus();
#else
    fxtm_gendata();
#endif
}

static void log()
{
#if 0
    loopSdcard();
#else
    fxtm_dump(false);
#endif
}

static void send()
{
#if 0
    loopRadio();
#endif
}

void subLoop()
{
    prof_start();

    acquire();
    log();
    send();

    prof_report();
}

#define BGC_ACQ_PERIOD 100 // in millis on each second.

void setup()
{
    setupInit();
    setupShell();
    setupRTC();
    setupRadio();
    setupPropellant();
    setupAtmos();
#if 0
    setupGps();
    setupIMU();
    setupSdcard();
#endif
    Init_Finish();
    TTRACE("#########################\n\r");
	TTRACE("Start transfer fxtm_data size:%d\n\r", fxtm_getdatasize());
    scheduler.setTimer(BGC_ACQ_PERIOD, subLoop, scheduler.RUN_FOREVER);
}

void loop()
{
    loopShell();
    scheduler.run();
}
