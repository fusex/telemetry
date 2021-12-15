/*
 * =====================================================================================
 *
 *       Filename:  sdcard2.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  28/05/2017 15:13:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#define TAG "SD"

#include <fusexconfig.h>
#include <fusexutil.h>

#include "FreeStack.h"
#include <SPI.h>
#include "SdFat.h"

#include "init.h"
#include "trame.h"
#include "pinout.h"
#include "sdcard.h"
#include "rtc.h"
#include "gps.h"

#define LOGFILENAME "fusexlog"
#define error(msg) {SD.errorPrint(&Serial, F(msg));}
#if 1
#define FILE_BLOCK_COUNT (60*60*10L) // 60 minutes logging
#else
#define FILE_BLOCK_COUNT (5*60*10L)    // 01 minutes logging
#endif

SdFat      SD;
SdBaseFile binFile;

uint32_t   bn = 0;  
uint32_t   maxLatency = 0;
uint16_t   fileid = 0;
uint16_t   filepart = 0;

#define BOOTID_DEFAULT_MASK 0xffff0000
#define BOOTID_DEFAULT      0xfc00

#define DEBUG

static void createBinFile()
{
    // max number of blocks to erase per erase call
    const uint32_t ERASE_SIZE = 262144L;
    uint32_t bgnBlock, endBlock;
    char filename[128];
    memset(filename,0,128);
    TTRACE("End createbinfile\r\n");

    if(isGPSFixed){
        char date[32]; 
	getGPSDateTime(date);
        sprintf(filename,"%s-%s-%d.txt",LOGFILENAME, date, filepart++,".txt");
    } else {
	fileid = (uint16_t)(getBootID() & 0x0000ffff);
	if((fileid & BOOTID_DEFAULT) == BOOTID_DEFAULT)
	    fileid = _myrandom(0,0xffff);
	sprintf(filename,"%s-%x-%d.txt",LOGFILENAME, fileid, filepart++,".txt");
    }

    // Delete old tmp file.
    if (SD.exists(filename)) {
#if 1
	TTRACE("Deleting tmp file:%s\r\n",filename);
	if (!SD.remove(filename))
#endif
	    error("filename already exist");
    }
    // Create new file.
    TTRACE("Creating new file\r\n");
    binFile.close();
    if (!binFile.createContiguous(filename, 512 * FILE_BLOCK_COUNT)) {
	error("createContiguous failed");
    }
    // Get the address of the file on the SD.
    if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
	error("contiguousRange failed");
    }

    // Flash erase all data in the file.
    TTRACE("Erasing all data\r\n");
    uint32_t bgnErase = bgnBlock;
    uint32_t endErase;
    while (bgnErase < endBlock) {
	endErase = bgnErase + ERASE_SIZE;
	if (endErase > endBlock) {
	    endErase = endBlock;
	}
	if (!SD.card()->erase(bgnErase, endErase)) {
	    error("erase failed");
	}
	bgnErase = endErase + 1;
    }

    if (!SD.card()->writeStart(binFile.firstBlock())) {
	error("writeStart failed");
    }
    bn = 0;  
    TTRACE("End createbinfile\r\n");
}

static int setupLowSD()
{
    TTRACE("Start setuplowsd\r\n");
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    if (!SD.begin(SD_CS_PIN)) {
	TTRACE("end -1 setuplowsd\r\n");
	return -1;
    }
    TTRACE("end 0 setuplowsd\r\n");
    return 0;
}

static void recordBinFile()
{
    fxtm_block_t* pBlock = fxtm_getblock();
    TTRACE("b--------------------------------------\r\n");
    SD.card()->spiStart();
    if (!SD.card()->isBusy()) {
	// Write block to SD.
	uint32_t usec = micros();
	if (!SD.card()->writeData((uint8_t*)pBlock)) {
	    error("write data failed");
	}
	usec = micros() - usec;
	if (usec > maxLatency) {
	    maxLatency = usec;
	}
	TTRACE("Block writed in usec: %ld\r\n", usec);
	bn++;
	if (bn == FILE_BLOCK_COUNT) {
	    // File full so stop
	    if (!SD.card()->writeStop()) {
		error("writeStop failed");
	    }
	    TTRACE("Max block write usec: %ld\r\n", maxLatency);
	    TTRACE("File limit reached ! recycle\r\n");
	    createBinFile(); 
	}
    } else
	WTTRACE("SDCard busy\r\n");

    SD.card()->spiStop();
    TTRACE("e--------------------------------------\r\n");
}

//TODO rotate filelog

void setupSdcard()
{
    if(setupLowSD()) {
	Init_SetSemiFatal();
	TTRACE("init Failed!\r\n");
	SD.card()->spiStop();
	return;
    }
    createBinFile(); 
    TTRACE("init Done.\r\n");
    SD.card()->spiStop();
}

void loopSdcard()
{
    recordBinFile();
}
