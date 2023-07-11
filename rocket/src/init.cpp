#define TAG "INIT"

#include <fusexutil.h>
#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <trame.h>
#include <BGC_Pinout.h>

#include "rtc.h"
#include "version.h"

#define MAXFATAL  4
#define MAXRESET 10

static bool    initfailed = false;
static uint8_t semifatalcount = 0;
static bool    skip = true;

void setupInit ()
{
    module_add(TAG);

    MCUSR = 0; // clear out any flags of prior resets.
    randomSeed(analogRead(0));
    DEBUGdevice.begin(DEBUGSERIALBAUD);

    pinMode(BGC_LED4_FERROR, OUTPUT);
    digitalWrite(BGC_LED4_FERROR, HIGH);

#ifndef CONFIG_PROD
    while (!DEBUGdevice) ; // Wait for DEBUG serial port to be available
#endif

    pinMode(BGC_LORA_SS, OUTPUT);
    digitalWrite(BGC_LORA_SS, HIGH);

    pinMode(BGC_SD_CS, OUTPUT);
    digitalWrite(BGC_SD_CS, HIGH);

    pinMode(BGC_SPIFLASH_CS, OUTPUT);
    digitalWrite(BGC_SPIFLASH_CS, HIGH);

    Wire.setClock(400000);

    BOOTTRACE("#######################\r\n");
    BOOTTRACE("Rocket init Start\r\n");
    BOOTTRACE("    Version: %s\r\n", VERSION);

    module_setup(TAG, FXTM_SUCCESS);
}

void Init_ResetBoard ()
{
    //turn on the WatchDog and don't stroke it.
    wdt_enable(WDTO_15MS);
    while(1);
}

void Init_Finish ()
{
    uint16_t rid = RTC_GetResetID();

    if (rid>MAXRESET)
        RTC_ResetResetID();

    rid = RTC_GetResetID();

    if (!initfailed) {
        digitalWrite(BGC_LED4_FERROR, LOW);
        BOOTTRACE("Rocket init Done\r\n");
    } else {
        BOOTTRACE("Rocket init failed !\r\n");
        for(int i=0;i<100;i++) {
            digitalWrite(BGC_LED4_FERROR, LOW);
            delay(100);
            digitalWrite(BGC_LED4_FERROR, HIGH);
            delay(100);
        }

        BOOTTRACE("rid :%d and semifatalcount:%d!\r\n",rid,semifatalcount);

        if (semifatalcount == MAXFATAL && rid<MAXRESET) {
            RTC_SetResetID();
            BOOTTRACE("FATAL init detected! reset !\r\n");
            Init_ResetBoard();
        }
    }
    BOOTTRACE("#######################\r\n");
}

void Init_SetFailed ()
{
    initfailed = true;
}

void Init_SetSemiFatal ()
{
    initfailed = true;
    semifatalcount++;
}

void Init_SetFatal ()
{
    initfailed = true;
    semifatalcount = MAXFATAL;
    Init_Finish();
}
