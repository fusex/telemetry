#define TAG "RTC"

#include <Wire.h>
#include <MCP79412RTC.h>    // http://github.com/JChristensen/MCP79412RTC
#include <TimeLib.h>        // https://www.pjrc.com/teensy/td_libs_DS1307RTC.html
#include <fusexutil.h>

#include "init.h"

#if 1
#define RTC_DEBUG 1
#endif

#define EEPROM_BOOT_OFFSET  2
#define EEPROM_RESET_OFFSET 0

#define SRAM_BOOT_OFFSET    10
#define SRAM_RESET_OFFSET   0

static uint32_t bootID  = 0;
static uint16_t resetID = 0;

uint32_t RTC_GetBootID()
{
    return bootID;
}

uint16_t RTC_GetResetID()
{
    return resetID;
}

void RTC_ResetResetID()
{
    RTC.sramWrite(SRAM_RESET_OFFSET, 0);
}

void RTC_SetResetID()
{
    RTC.sramWrite(SRAM_RESET_OFFSET, resetID+1);
}

void RTC_DumpDebug(bool isConsole)
{
    time_t t = now();

    MYTTRACE("resetId: %d.\r\n", resetID);
    MYTTRACE("bootId: %d.\r\n", bootID);
    MYTTRACE("date and time: ");

    MYTRACE("%2d:%2d:%2d %s %d %s %4d\r\n",
		hour(t),
		minute(t),
		second(t),
		dayShortStr(weekday(t)),
		day(t),
		monthShortStr(month(t)),
		year(t)
	  );
}

static time_t RTC_GetCompileTime()
{
    const uint32_t FUDGE(15);        // fudge factor to allow for compile time (seconds, YMMV)
    tmElements_t tm;
    time_t       t;

    const char *compDate = __DATE__,
               *compTime = __TIME__,
               *months = "JanFebMarAprMayJunJulAugSepOctNovDec";

    char chMon[3], *m;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);

    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    t = makeTime(tm);

    return t + FUDGE;        // add fudge factor to allow for compile time
}

void setupRTC()
{
    module_add(TAG);
#if 1
    setSyncProvider(RTC.get); // the function to get the time from the RTC
#else
    setTime(RTC_GetCompileTime()); // set the system time to the sketch compile time
    RTC.set(now());                // set the RTC from the system time
    RTC.sramWrite(SRAM_RESET_OFFSET, 0);
    RTC.sramWrite(SRAM_BOOT_OFFSET, 0);
#endif

    RTC.vbaten(true);

    bootID  = RTC.sramRead(SRAM_BOOT_OFFSET);
    resetID = RTC.sramRead(SRAM_RESET_OFFSET);

    RTC.sramWrite(SRAM_BOOT_OFFSET, bootID+1);

#if RTC_DEBUG
    RTC_DumpDebug(false);
#endif

    TTRACE("init Done.\r\n");
    module_setup(TAG, FXTM_SUCCESS);
}

void loopRTC()
{
}
