#define BOARD_MEGA 1

#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"

#include <fusexutil.h>
#include "trame.h"
#include "sdcard.h"

int dump = 0;

void setup()
{
    setupCommon();
    setupRadio();
    setupIMU();
    setupGps();
    setupPropellant();
    setupSdcard();
}

static void acquire()
{
    fxtm_reset();

    loopPropellant();
    loopIMU();
    loopGps();

    if(dump) fxtm_dump(); 
}

static void log()
{
    loopSdcard();
}

static void send()
{
    loopRadio();
}

uint32_t max = 0;
uint32_t avg = 0;
uint16_t count = 0;
uint32_t timer = millis();

void loop()
{
#if 0
    uint32_t time = micros();
    uint32_t d1 = 0;
#endif

    if (timer > millis())  timer = millis();
    if (millis() - timer > 2000) {
	timer = millis(); // reset the timer
	dump = 1;
    }

    acquire();
    log();
    send();
    dump = 0;

    return;

#if 0
    d1 = micros() - time;
    if(d1>max) max = d1; 
    avg += d1;

#define LOOP 200000UL
//#define LOOP 1000
delay(1000);

    if(count++>LOOP) {
	TTRACE("%ld loop in:%lu (avg:%lu, max:%lu)\r\n", LOOP, d1 ,avg/(LOOP+1), max);
	while(1);
    }
#if 0
    TTRACE("id:%u loop in :%ld\r\n", fxtmblock.data.id, micros()-time);
    delay(100);
#endif
#endif
}
