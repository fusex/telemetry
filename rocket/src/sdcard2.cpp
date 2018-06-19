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

#include <fusexconfig.h>
#include <fusexutil.h>

#include "FreeStack.h"
#include <SPI.h>
#include "SdFat.h"

#include "common.h"
#include "trame.h"
#include "pinout.h"
#include "sdcard.h"

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

static void genfileid()
{
    fileid = _myrandom(0,0xffff);
}

static void createBinFile()
{
    // max number of blocks to erase per erase call
    const uint32_t ERASE_SIZE = 262144L;
    uint32_t bgnBlock, endBlock;
    char filename[128];
    memset(filename,0,128);

    sprintf(filename,"%s-%x-%d.txt",LOGFILENAME, fileid, filepart++,".txt");

    // Delete old tmp file.
    if (SD.exists(filename)) {
#if 1
	TTRACE("Deleting tmp file:%s\r\n",filename);
	if (!SD.remove(filename))
#endif
	    error("filename already exist");
    }
    // Create new file.
    WTTRACE("Creating new file\r\n");
    binFile.close();
    if (!binFile.createContiguous(filename, 512 * FILE_BLOCK_COUNT)) {
	error("createContiguous failed");
    }
    // Get the address of the file on the SD.
    if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
	error("contiguousRange failed");
    }

    // Flash erase all data in the file.
    WTTRACE("Erasing all data\r\n");
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
}

static int setupLowSD()
{
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    if (!SD.begin(SD_CS_PIN)) {
	return -1;
    }
    return 0;
}

static void recordBinFile()
{
    fxtm_block_t* pBlock = fxtm_getblock();
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
	    TTRACE("File limit reached ! recycle\r\n");
	    createBinFile(); 
	}
    } else
	WTTRACE("SDCard busy\r\n");

    SD.card()->spiStop();
    WTTRACE("e--------------------------------------\r\n");
}

//TODO rotate filelog

void setupSdcard()
{
    genfileid();

    if(setupLowSD()) {
	setupSetFailed();
	TTRACE("initialization failed!\r\n");
	return;
    }
    createBinFile(); 
    TTRACE("initialization done.\r\n");
}

void loopSdcard()
{
    recordBinFile();
}
