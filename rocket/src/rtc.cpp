#define TAG "RTC"

#include <fusexutil.h>
#include <Wire.h>
#include "RTClib.h"
#include "common.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#if 0
#define RTC_DEBUG
#endif

#define EEPROM_BOOT_OFFSET 2
#define EEPROM_RESET_OFFSET 0

static uint32_t bootID  = 0;
static uint16_t resetID = 0;

uint32_t getBootID()
{
  return bootID; 
}

uint16_t getResetID()
{
  return resetID; 
}

void setResetResetID()
{
   resetID = 0;
   rtc.writenvram(EEPROM_RESET_OFFSET, (uint8_t*)&resetID, sizeof(resetID));
}

void setResetID()
{
    resetID++; 
    rtc.writenvram(EEPROM_RESET_OFFSET, (uint8_t*)&resetID, sizeof(resetID));
}

static void debugRTC()
{
#if RTC_DEBUG
    DateTime now = rtc.now();
    
    PRINT(now.year(), DEC);
    PRINT('/');
    PRINT(now.month(), DEC);
    PRINT('/');
    PRINT(now.day(), DEC);
    PRINT(" (");
    PRINT(daysOfTheWeek[now.dayOfTheWeek()]);
    PRINT(") ");
    PRINT(now.hour(), DEC);
    PRINT(':');
    PRINT(now.minute(), DEC);
    PRINT(':');
    PRINT(now.second(), DEC);
    PRINTLN();
    
    PRINT(" since midnight 1/1/1970 = ");
    PRINT(now.unixtime());
    PRINT("s = ");
    PRINT(now.unixtime() / 86400L);
    PRINTLN("d");
    
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
    
    PRINT(" now + 7d + 30s: ");
    PRINT(future.year(), DEC);
    PRINT('/');
    PRINT(future.month(), DEC);
    PRINT('/');
    PRINT(future.day(), DEC);
    PRINT(' ');
    PRINT(future.hour(), DEC);
    PRINT(':');
    PRINT(future.minute(), DEC);
    PRINT(':');
    PRINT(future.second(), DEC);
    PRINTLN();
    
    PRINTLN();
    uint8_t readData[8];
    rtc.readnvram(readData, 8, 0);
    PRINTLN("Reading NVRAM values:");
    PRINTLN(readData[0], HEX);
    PRINTLN(readData[1], HEX);
    PRINTLN(readData[2], HEX);
    PRINTLN(readData[3], HEX);
    PRINTLN(readData[4], HEX);
    PRINTLN(readData[5], HEX);
    PRINTLN(readData[6], HEX);
    PRINTLN(readData[7], HEX);
#endif
}

void setupRTC()
{
  if (!rtc.begin()) {
      PRINTLN("Couldn't find RTC");
      TTRACE("init failed ! \n\r");
      setupSetFailed();
      return;
  }
  if (!rtc.isrunning()) {
      PRINTLN("RTC is NOT running!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // following line sets the RTC to the date & time this sketch was compiled
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  debugRTC();
  uint8_t* ptr;

  ptr = (uint8_t*)&resetID;
  rtc.readnvram(ptr, sizeof(resetID), EEPROM_RESET_OFFSET);
  TTRACE("RESET Id: 0x%x.\r\n", resetID);

  ptr = (uint8_t*)&bootID;
  rtc.readnvram(ptr, sizeof(bootID), EEPROM_BOOT_OFFSET);
  TTRACE("Boot Id: 0x%lx.\r\n", bootID);


  bootID++;
  rtc.writenvram(EEPROM_BOOT_OFFSET, ptr, sizeof(bootID));

  TTRACE("init Done.\r\n");
  debugRTC();
}

static uint32_t rtcloop = 0;

void loopRTC()
{
    debugRTC();
#if 0
    if(!(rtcloop&0xff)) {
	uint32_t time = micros();
	rtc.adjust(rtc.now());
	uint32_t d1 = micros() - time;
	TTRACE("adjust in:%lu\r\n", d1);
    }
#endif
}
