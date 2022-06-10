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
#include "shell.h"

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
}

void subloop()
{
    prof_start();
    acquire();
    log();
    send();
    prof_report();
}

void loop() {
    loopShell();
}
