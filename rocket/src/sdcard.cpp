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

#include "FreeStack.h"
#include <SPI.h>
#include "SdFat.h"

#include "trame.h"

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
#define FILE_BASE_NAME "adc4pin"

#define error(msg) {SD.errorPrint(&Serial, F(msg));while(1);}

const uint32_t LOG_INTERVAL_USEC = 2000;
const uint32_t FILE_BLOCK_COUNT = 256000;
#define TMP_FILE_NAME FILE_BASE_NAME "##.bin"
const uint8_t BUFFER_BLOCK_COUNT = 10;

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
const uint8_t FILE_NAME_DIM  = BASE_NAME_SIZE + 7;
char binName[FILE_NAME_DIM] = FILE_BASE_NAME "00.bin";

SdBaseFile binFile;

// Number of data records in a block.
const uint16_t DATA_DIM = (512 - 4)/sizeof(fxtm_data_t);

//Compute fill so block size is 512 bytes.  FILL_DIM may be zero.
const uint16_t FILL_DIM = 512 - 4 - DATA_DIM*sizeof(fxtm_data_t);

struct block_t {
  uint16_t count;
  uint16_t overrun;
  fxtm_data_t data[DATA_DIM];
  uint8_t fill[FILL_DIM];
};


void createBinFile() {
  // max number of blocks to erase per erase call
  const uint32_t ERASE_SIZE = 262144L;
  uint32_t bgnBlock, endBlock;
  
  // Delete old tmp file.
  if (SD.exists(TMP_FILE_NAME)) {
    Serial.println(F("Deleting tmp file " TMP_FILE_NAME));
    if (!SD.remove(TMP_FILE_NAME)) {
      error("Can't remove tmp file");
    }
  }
  // Create new file.
  Serial.println(F("\nCreating new file"));
  binFile.close();
  if (!binFile.createContiguous(TMP_FILE_NAME, 512 * FILE_BLOCK_COUNT)) {
    error("createContiguous failed");
  }
  // Get the address of the file on the SD.
  if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
    error("contiguousRange failed");
  }
  // Flash erase all data in the file.
  Serial.println(F("Erasing all data"));
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
}

// Acquire a data record.
void acquireData(fxtm_data_t* data) {
  uint8_t* p = (uint8_t*)data;
  gendata(p,sizeof(fxtm_data_t));
  data->timestamp = micros();
}

//------------------------------------------------------------------------------
void recordBinFile() {
  const uint8_t QUEUE_DIM = BUFFER_BLOCK_COUNT + 1;
  // Index of last queue location.
  const uint8_t QUEUE_LAST = QUEUE_DIM - 1;
  
  // Allocate extra buffer space.
  block_t block[BUFFER_BLOCK_COUNT - 1];
  
  block_t* curBlock = 0;
  
  block_t* emptyStack[BUFFER_BLOCK_COUNT];
  uint8_t emptyTop;
  uint8_t minTop;

  block_t* fullQueue[QUEUE_DIM];
  uint8_t fullHead = 0;
  uint8_t fullTail = 0;  

  // Use SdFat's internal buffer.
  emptyStack[0] = (block_t*)SD.vol()->cacheClear();
  if (emptyStack[0] == 0) {
    error("cacheClear failed");
  }
  // Put rest of buffers on the empty stack.
  for (int i = 1; i < BUFFER_BLOCK_COUNT; i++) {
    emptyStack[i] = &block[i - 1];
  }
  emptyTop = BUFFER_BLOCK_COUNT;
  minTop = BUFFER_BLOCK_COUNT;
  
  // Start a multiple block write.
  if (!SD.card()->writeStart(binFile.firstBlock())) {
    error("writeStart failed");
  }
  Serial.print(F("FreeStack: "));
  Serial.println(FreeStack());
  Serial.println(F("Logging - type any character to stop"));
  bool closeFile = false;
  uint32_t bn = 0;  
  uint32_t maxLatency = 0;
  uint32_t overrun = 0;
  uint32_t overrunTotal = 0;
  uint32_t logTime = micros();
  while(1) {
     // Time for next data record.
    logTime += LOG_INTERVAL_USEC;
    if (Serial.available()) {
      closeFile = true;
    }  
    if (closeFile) {
      if (curBlock != 0) {
        // Put buffer in full queue.
        fullQueue[fullHead] = curBlock;
        fullHead = fullHead < QUEUE_LAST ? fullHead + 1 : 0;
        curBlock = 0;
      }
    } else {
      if (curBlock == 0 && emptyTop != 0) {
        curBlock = emptyStack[--emptyTop];
        if (emptyTop < minTop) {
          minTop = emptyTop;
        }
        curBlock->count = 0;
        curBlock->overrun = overrun;
        overrun = 0;
      }
      if ((int32_t)(logTime - micros()) < 0) {
        //error("Rate too fast");             
      }
      int32_t delta;
      do {
        delta = micros() - logTime;
      } while (delta < 0);
      if (curBlock == 0) {
        overrun++;
        overrunTotal++;
#if ABORT_ON_OVERRUN
        Serial.println(F("Overrun abort"));
        break;
 #endif  // ABORT_ON_OVERRUN       
      } else {
#if USE_SHARED_SPI
        SD.card()->spiStop();
#endif  // USE_SHARED_SPI   
        acquireData(&curBlock->data[curBlock->count++]);
#if USE_SHARED_SPI
        SD.card()->spiStart();
#endif  // USE_SHARED_SPI      
        if (curBlock->count == DATA_DIM) {
          fullQueue[fullHead] = curBlock;
          fullHead = fullHead < QUEUE_LAST ? fullHead + 1 : 0;
          curBlock = 0;
        } 
      }
    }
    if (fullHead == fullTail) {
      // Exit loop if done.
      if (closeFile) {
        break;
      }
    } else if (!SD.card()->isBusy()) {
      // Get address of block to write.
      block_t* pBlock = fullQueue[fullTail];
      fullTail = fullTail < QUEUE_LAST ? fullTail + 1 : 0;
      // Write block to SD.
      uint32_t usec = micros();
      if (!SD.card()->writeData((uint8_t*)pBlock)) {
        error("write data failed");
      }
      usec = micros() - usec;
      if (usec > maxLatency) {
        maxLatency = usec;
      }
      // Move block to empty queue.
      emptyStack[emptyTop++] = pBlock;
      bn++;
      if (bn == FILE_BLOCK_COUNT) {
        // File full so stop
        break;
      }
    }
  }
  if (!SD.card()->writeStop()) {
    error("writeStop failed");
  }
  Serial.print(F("Min Free buffers: "));
  Serial.println(minTop);
  Serial.print(F("Max block write usec: "));
  Serial.println(maxLatency);
  Serial.print(F("Overruns: "));
  Serial.println(overrunTotal);
  // Truncate file if recording stopped early.
  if (bn != FILE_BLOCK_COUNT) {
    Serial.println(F("Truncating file"));
    if (!binFile.truncate(512L * bn)) {
      error("Can't truncate file");
    }
  }
}


void renameBinFile() {
  while (SD.exists(binName)) {
    if (binName[BASE_NAME_SIZE + 1] != '9') {
      binName[BASE_NAME_SIZE + 1]++;
    } else {
      binName[BASE_NAME_SIZE + 1] = '0';
      if (binName[BASE_NAME_SIZE] == '9') {
        error("Can't create file name");
      }
      binName[BASE_NAME_SIZE]++;
    }
  }
  if (!binFile.rename(SD.vwd(), binName)) {
    error("Can't rename file");
    }
  Serial.print(F("File renamed: "));
  Serial.println(binName);
  Serial.print(F("File size: "));
  Serial.print(binFile.fileSize()/512);
  Serial.println(F(" blocks"));
}


// log data
void logData() {
  createBinFile();
  recordBinFile();
  renameBinFile();
}


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
    logData();
#endif
}

void setupSdcard()
{
#ifndef CONFIG_SDCARD
    return;
#endif

    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

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
