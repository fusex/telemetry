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
    loopRTC();
#if 0
    loopPropellant();
    loopAtmos();
    loopGps();
    loopIMU();
#else
    fxtm_gendata();
#endif
}

static void log()
{
#if 0
    loopSdcard();
#else
    fxtm_dump();
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
#if 0
    acquire();
    log();
    send();
#else
    delay(100);
#endif

    prof_report();
}

#define BGC_ACQ_PERIOD 1000 // in millis on each second.

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
