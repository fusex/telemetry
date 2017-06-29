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

#define TAG "SDCARD"

#if 0
#include <fusexconfig.h>
#include <fusexutil.h>

#include "FreeStack.h"
#include <SPI.h>
#include "SdFat.h"

#include "trame.h"
#include "sdcard.h"

#if 0
#define SD_CS_PIN SS
#elif 0
#define SD_CS_PIN 10
#else
#define SD_CS_PIN 4
#endif

#define LOGFILENAME "fusex02.txt"
#define error(msg) {SD.errorPrint(&Serial, F(msg));while(1);}
#if 0
#define FILE_BLOCK_COUNT (60*60*10L) // 60 minutes logging
#else
#define FILE_BLOCK_COUNT (60*10L)    // 01 minutes logging
#endif

SdFat      SD;
SdBaseFile binFile;

uint32_t   bn = 0;  
uint32_t   maxLatency = 0;

extern block_t fxtmblock;

void createBinFile()
{
    // max number of blocks to erase per erase call
    const uint32_t ERASE_SIZE = 262144L;
    uint32_t bgnBlock, endBlock;

    // Delete old tmp file.
    if (SD.exists(LOGFILENAME)) {
	TTRACE("Deleting tmp file " LOGFILENAME "\r\n");
	if (!SD.remove(LOGFILENAME)) {
	    error("Can't remove tmp file");
	}
    }
    // Create new file.
    TTRACE("Creating new file\r\n");
    binFile.close();
    if (!binFile.createContiguous(LOGFILENAME, 512 * FILE_BLOCK_COUNT)) {
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
}

void setupLowSD()
{
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    if (!SD.begin(SD_CS_PIN)) {
	TTRACE("SDCard: initialization failed!\r\n");
	while(1);
    }
    TTRACE("SDCard: initialization done.\r\n");
}

void recordBinFile()
{
    block_t* pBlock = &fxtmblock;
    WTTRACE("b--------------------------------------\r\n");
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
	WTTRACE("Block writed in usec: %ld\r\n", usec);
	bn++;
	if (bn == FILE_BLOCK_COUNT) {
	    // File full so stop
	    if (!SD.card()->writeStop()) {
		error("writeStop failed");
	    }
	    TTRACE("Max block write usec: %ld\r\n", maxLatency);
	    TTRACE("File limit reached ! abort\r\n");
	    while(1);
	}
    } else
	TTRACE("SDCard busy\r\n");

    SD.card()->spiStop();
    WTTRACE("e--------------------------------------\r\n");
}

//TODO rotate filelog

void setupSdcard()
{
    setupLowSD();
    createBinFile(); 
}

void loopSdcard()
{
    recordBinFile();
}
#else
void setupSdcard()
{
}

void loopSdcard()
{
}
#endif
