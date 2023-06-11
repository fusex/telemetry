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
#elif 0
    fxtm_setgps(48.866667, 2.333333);
    fxtm_setflightstatus(FXTM_FLIGHTSTATUS_SEPARATION);
    fxtm_setpressure(100);
    fxtm_setdiffpressure(200);
    fxtm_settemperature(-5);
    fxtm_sethumidity(98);
    float a[]  = { 1.11,  2.22,  3.33};
    float m[] =  { 4.44,  5.55,  6.66};
    float g[] =  { 7.77,  8.88,  9.99};
    float a2[] = {10.10, 11.11, 12.12};
    fxtm_setimu(a, m, g);
    fxtm_setimu2(a2);
#else
    memset((uint8_t*)fxtm_getdata(), 0xaa, fxtm_getdatasize());
#endif

    fxtm_reset(_mymillis());
}

static void acquire (void)
{
    fxtm_reset(_mymillis());
    loopRTC();
    loopPitot();
    loopAtmos();
    loopIMU();
#ifdef ENABLE_GPS
    loopGps();
#endif
}

static void log (void)
{
    loopSdcard();
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
    log();
#else
    acquire_fake();
#endif

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
    setupSdcard();
#endif

    setupRadio();

    setupExec();

    Init_Finish();

    TTRACE("#########################\n\r");
    TTRACE("Start transfer fxtm_data size:%d\n\r", fxtm_getdatasize());
}

void loop (void)
{
    loopShell();
    loopExec();
}
