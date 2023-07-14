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
#include "flash.h"

#if 1
#define ENABLE_GPS
#endif

#if 0
#define ENABLE_FAKE
#endif

static void acquire_fake (void)
{
#if 0
    gendata((uint8_t*)fxtm_getdata(), fxtm_getdatasize());
    fxtm_reset(_mymillis());
#elif 1
    fxtm_reset(_mymillis());
    fxtm_setgps(48, 2);
    fxtm_setflightstatus(FXTM_FLIGHTSTATUS_SEPARATION);
    fxtm_setpressure(100);
    fxtm_setdiffpressure(200);
    fxtm_settemperature(-5);
    fxtm_sethumidity(98);
    fxtm_seterror(FXTM_SDCARD);
    imuraw_t a[]  = { 111,  222,  333};
    imuraw_t m[]  = { 444,  555,  666};
    imuraw_t g[]  = { 777,  888,  999};
    imuraw_t a2[] = {1010, 1111, 1212};
    fxtm_setimu(a, m, g);
    fxtm_setimu2(a2);
#else
    memset((uint8_t*)fxtm_getdata(), 0xaa, fxtm_getdatasize());
    fxtm_reset(_mymillis());
#endif
}

static void acquire (void)
{
    fxtm_reset(_mymillis());
    loopRTC();
    loopAtmos();
    loopIMU();
    loopPitot();
#ifdef ENABLE_GPS
    loopGps();
#endif
}

static void log (void)
{
    loopSdcard();
    loopFlash();
}

static void send (void)
{
    loopRadio();
}

void subLoop (void)
{
    prof_start();

#ifndef ENABLE_FAKE
    acquire();
#else
    acquire_fake();
#endif

    log();
    send();

    prof_report();
}

void setup (void)
{
    setupInit();
    setupShell();

#ifndef ENABLE_FAKE
    setupRTC();
    setupPitot();
    setupAtmos();
    setupIMU();
#ifdef ENABLE_GPS
    setupGps();
#endif
#endif

    setupSdcard();
    setupFlash();
    setupRadio();

    setupExec();

#ifndef CONFIG_DONT_WAIT
    Init_Finish();
#endif

    BOOTTRACE("#########################\n\r");
    BOOTTRACE("Start transfer fxtm_data size:%d\n\r", fxtm_getdatasize());
}

void loop (void)
{
    loopShell();
    loopExec();
}
