#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"

#if 0
#include <pt.h>   // include protothread library
static struct pt pt1, pt2, pt3; // each protothread needs one of these
#endif

void setup()
{
    setupCommon();
    setupRadio();
    setupImu();
    setupGps();
    setupPropellant();
    setupSdcard();
}

#if 0
static int acquire(struct pt* pt, int interval)
{
    static unsigned long timestamp = 0;
    PT_BEGIN(pt);
    while(1) {
        PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
        timestamp = millis();
	loopPropellant();
	loopImu();
	loopGps();

    }
    PT_END(pt);
}

static int log(struct pt* pt, int interval)
{
    static unsigned long timestamp = 0;
    PT_BEGIN(pt);
    while(1) {
        PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
        timestamp = millis();
	loopSdcard();
    }
    PT_END(pt);
#}

static int send(struct pt* pt, int interval)
{
    static unsigned long timestamp = 0;
    PT_BEGIN(pt);
    while(1) {
       // PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
        //	timestamp = millis();
        int idx = 2;  
        PT_WAIT_UNTIL(pt, !fxtmbuff[idx].locked );
        fxtmbuff[idx].status = FXTM_SENDING;
	loopRadio();
        fxtmbuff[idx].locked = false;
    }
    PT_END(pt);
}

void loop()
{
  acquire(&pt1, 10);
  log(&pt2, 10);
  send(&pt3, 80);
}
#else

static int acquire()
{
    loopPropellant();
    loopImu();
    loopGps();
}

static int log()
{
    loopSdcard();
}

static int send()
{
    loopRadio();
}

void loop()
{
  acquire();
  log();
  send();
}
#endif
