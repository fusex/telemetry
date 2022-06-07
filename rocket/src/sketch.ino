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

#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"
#include "debug.h"

static void acquire()
{
    fxtm_reset();
    loopRTC();
#if 1
    loopPropellant();
    loopAtmos();
    loopGps();
#else
    loopIMU();
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
    loopRadio();
}

void setup()
{
    setupInit();
    setupRTC();
    setupRadio();
    setupPropellant();
    setupAtmos();
    setupGps();
#if 0
    setupIMU();
    setupSdcard();
#endif
    Init_Finish();
    TTRACE("#########################\n\r");
	TTRACE("Start transfer fxtm_data size:%d\n\r", fxtm_getdatasize());
}

void loop()
{
    prof_start();
    acquire();
    log();
#if 0
    send();
#endif
    prof_report();
    delay(10000);
}

