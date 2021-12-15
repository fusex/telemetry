#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"
#include "rtc.h"
#include "prof.h"

#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"

static void acquire()
{
    fxtm_reset();
    loopIMU();
    loopGps();

    loopPropellant();
    loopRTC();

#ifdef DEBUG
    fxtm_dump(NULL); 
#endif
}

static void log()
{
    loopSdcard();
}

static void send()
{
    loopRadio();
}

void setup()
{
    setupCommon();
    setupRTC();
    setupRadio();
    setupIMU();
    setupGps();
    setupPropellant();
    setupSdcard();
    setupFinishCommon();

    TTRACE("#########################\n\r");
	TTRACE("Start transfer packet size:%d\r\n",fxtm_getdatasize());
}

void loop()
{
    prof_start();
    acquire();
    log();
    send();
    prof_report();
}
