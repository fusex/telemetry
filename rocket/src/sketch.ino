#define TAG "MAIN"

#include <SimpleTimer.h>
#include <fusexutil.h>
#include <trame.h>

#include "init.h"
#include "imu.h"
#include "atmos.h"
#include "pitot.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "rtc.h"
#include "prof.h"
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
    loopAtmos();
    loopPitot();
    loopIMU();
    loopGps();
}

static void log()
{
    loopSdcard();
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

#define BGC_ACQ_PERIOD 100 // in millis on each second.

void setup()
{
    setupInit();
    setupShell();
    setupRTC();
    setupAtmos();
    setupIMU();
    setupPitot();
    setupRadio();
    setupGps();
    setupSdcard();
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
