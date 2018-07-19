#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"
#include "rtc.h"

#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"

static bool once = true;

#ifdef PROFILING
static uint32_t max = 0;
static uint32_t avg = 0;
static uint32_t count = 0;
#define LOOP 200000UL
//#define LOOP 1000
#endif

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
}


void loop()
{
    if(once){
	TTRACE("#########################\n\r");
	TTRACE("Start transfer packet size:%d\r\n",fxtm_getdatasize());
        once = false;
    }

#ifdef PROFILING
    uint32_t time = micros();
#endif

    acquire();
    log();
    send();

#ifdef PROFILING 
    uint32_t d1 = micros() - time;
    if(d1>max) max = d1; 
    avg += d1;

    if(count++>LOOP) {
	TTRACE("%ld loop in:%lu (avg:%lu, max:%lu)\r\n", count, d1 ,avg/(count+1), max);
    }
#endif
}
