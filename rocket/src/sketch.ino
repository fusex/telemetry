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

static void acquire()
{
    fxtm_reset();
    loopIMU();
    loopGps();

    loopPropellant();
    loopRTC();
    loopAtmos();

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
    setupInit();
    setupRTC();
    setupRadio();
    setupIMU();
    setupGps();
    setupPropellant();
    setupAtmos();
    setupSdcard();

    Init_Finish();
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
