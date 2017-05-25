/*
 * =====================================================================================
 *
 *       Filename:  sdcard.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:23:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#include <fusexconfig.h>
#include <fusexutil.h>

#include <SPI.h>
#include "SdFat.h"

#if 0
#define SD_CS_PIN SS
#else
#define SD_CS_PIN 4
#endif

#define LOGFILENAME "fusex.txt"

SdFat SD;
File myFile;

#ifdef CONFIG_TEST_SDCARD
void testSdCard1()
{
    uint8_t       data[251];
    unsigned long d1, time;

    TTRACE("SDCard: Test Writing to file :%s ...\r\n", LOGFILENAME);
    gendata(data, sizeof(data));

    time = micros();
    myFile.write(data,42);
    d1 = micros() - time;
    myFile.flush();

    TTRACE("SDCard: packet logged in: %ld us and flushed in: %ld us\r\n",
	   d1, micros()-time);
}

void testSdCard2()
{
    uint8_t       data[251];
    unsigned long d1, time, avg, max=0, min=0xfffffff;

    TTRACE("SDCard: Test2 Writing to file :%s ...\r\n", LOGFILENAME);
    gendata(data, sizeof(data));

    unsigned int count = 0;
    unsigned int mcount = 0;
    unsigned int expired= 0;

    /* simulate a log duration of 3 minutes at 10 hz*/
    while(count++ < 3*60*10){
	time = micros();
	myFile.write(data, 42);
	d1 = micros() - time;

        if (d1>max) {mcount= count-1; max = d1;}
        if (d1<min) min = d1;
#define EXPECTED 6000
	if(d1>EXPECTED) expired++;
        avg+=d1;
        delay(90);
    }
    TTRACE("SDCard: %d packet logged in:\r\n"
	    "total:   %ld\r\n"
	    "avg:     %ld\r\n" 
	    "min:     %ld\r\n" 
	    "max:     %ld (at iter:%d)\r\n"
	    "expired: %d\r\n",
	     count-1, avg,avg/1000, min, max, mcount, expired);
    myFile.flush();
}

void testSdCard3()
{
    uint8_t       data[251];
    unsigned long d1, time, avg, max=0, min=0xfffffff;

    TTRACE("SDCard: Test3 Writing to file :%s ...\r\n", LOGFILENAME);
    gendata(data, sizeof(data));

    unsigned int count = 0;
    unsigned int mcount = 0;
    unsigned int expired= 0;

    while(count++ < 100){
	int i = 0;
	time = micros();
	while(i++<count)
	    myFile.write(data, 42);

	myFile.flush();
	d1 = micros() - time;

	TTRACE("SDCard: %d*42 packet flushed in: %ld\r\n", count, d1);
        //delay(500);
    }
}

void testSdCard4()
{
    uint8_t       data[251];
    unsigned long d1, time, avg, max=0, min=0xfffffff;

    TTRACE("SDCard: Test4 Writing to file :%s ...\r\n", LOGFILENAME);
    gendata(data, sizeof(data));

    unsigned int count = 0;
    unsigned int mcount = 0;
    unsigned int expired= 0;

    while(count++ < 3*60*10){
	int i = 0;
	time = micros();
	while(i++<10)
	    myFile.write(data, 42);

	myFile.flush();
	d1 = micros() - time;

	TTRACE("SDCard: %d*42 packet flushed in: %ld\r\n", count, d1);
        delay(90);
    }
}
#endif

void testSdCard()
{
#ifdef CONFIG_TEST_SDCARD
# if 0
    testSdCard1();
# endif
# if 0
    testSdCard2();
# endif
# if 0
    testSdCard3();
# endif
# if 0
    testSdCard4();
# endif
#endif
}

void setupSdcard()
{
#ifndef CONFIG_SDCARD
    return;
#endif

    pinMode(SD_CS_PIN, OUTPUT);

    if (!SD.begin(SD_CS_PIN)) {
	TTRACE("SDCard: initialization failed!\r\n");
	return;
    }
    TTRACE("SDCard: initialization done.\r\n");
    myFile = SD.open(LOGFILENAME, FILE_WRITE);
    if (!myFile) {
	TTRACE("SDCard: ERROR: Opening file %s !\r\n", LOGFILENAME);
	return;
    }

    testSdCard();
}

void loopSdcard()
{
#ifndef CONFIG_SDCARD
    return;
#endif
}
