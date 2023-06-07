#define TAG "MAIN"

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
#include "exec.h"

static void acquire_fake ()
{
    fxtm_reset(_mymillis());
    gendata((uint8_t*)fxtm_getdata(), fxtm_getdatasize());
}

static void acquire ()
{
    fxtm_reset(_mymillis());
    loopRTC();
    loopPitot();
    loopAtmos();
    loopIMU();

#if 1
    loopGps();
#endif
}

static void log ()
{
    loopSdcard();
}

static void send ()
{
    loopRadio();
}

void subLoop ()
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

void setup ()
{
    setupInit();
    setupShell();
    setupRTC();
    setupPitot();
    setupAtmos();
    setupIMU();
#if 1
    setupGps();
#endif
    setupSdcard();
    setupRadio();

    setupExec();

    Init_Finish();

    TTRACE("#########################\n\r");
    TTRACE("Start transfer fxtm_data size:%d\n\r", fxtm_getdatasize());
}

void loop ()
{
    loopShell();
    loopExec();
}
