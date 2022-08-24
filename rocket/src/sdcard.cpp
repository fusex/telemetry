#define TAG "SDCARD"

#include <fusexconfig.h>
#include <fusexutil.h>
#include <trame.h>
#include <BGC_Pinout.h>

#include <FreeStack.h>
#include <SPI.h>
#include <SdFat.h>
#include <TimeLib.h>        // https://www.pjrc.com/teensy/td_libs_DS1307RTC.html

#include "init.h"
#include "sdcard.h"

#define LOGFILENAME "fusexlog"
#define error(msg) {SD.errorPrint(&Serial, F(msg));}
#define FILE_BLOCK_COUNT (120*60*10L) // 120 minutes logging at 10HZ

static SdFat      SD;
static SdBaseFile binFile;

static uint32_t   bn = 0;  
static uint16_t   filepart = 0;
static char       filename[128];

#define BOOTID_DEFAULT_MASK 0xffff0000
#define BOOTID_DEFAULT      0xfc00

static void SD_CreateBinFile ()
{
    // max number of blocks to erase per erase call
    const uint32_t ERASE_SIZE = 262144L;
    uint32_t bgnBlock, endBlock;
    memset(filename, 0, 128);

    sprintf(filename, "%s-%d-%d-%d-%d-%d-%d-%d.bgc",
            LOGFILENAME,
            hour(), minute(), second(), day(), month(), year(),
            filepart++);

    // Create new file.
    binFile.close();

    uint32_t usec = micros();
    if (!binFile.createContiguous(filename, 512 * FILE_BLOCK_COUNT)) {
        error("createContiguous failed");
    }
    TTRACE("sdcard createBinFile1 in usec: %ld\r\n", micros() - usec);
    usec = micros();
    // Get the address of the file on the SD.
    if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
        error("contiguousRange failed");
    }

    TTRACE("sdcard createBinFile2 in usec: %ld\r\n", micros() - usec);

    // Flash erase all data in the file.
    usec = micros();
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
    TTRACE("sdcard createBinFile3 in usec: %ld\r\n", micros() - usec);

    usec = micros();
    if (!SD.card()->writeStart(binFile.firstBlock()))
        error("writeStart failed");
    TTRACE("sdcard createBinFile4 in usec: %ld\r\n", micros() - usec);

    bn = 0;
}

static int SD_Init ()
{
    pinMode(BGC_SD_CS, OUTPUT);
    digitalWrite(BGC_SD_CS, HIGH);

    if (!SD.begin(BGC_SD_CS))
        return -1;

    return 0;
}

static void SD_RecordBinFile ()
{
    fxtm_block_t *pBlock = fxtm_getblock();
    SD.card()->spiStart();
    if (!SD.card()->isBusy()) {
        // Write block to SD.
        if (!SD.card()->writeData((uint8_t *)pBlock)) {
            error("write data failed");
        }
        bn++;
        if (bn == FILE_BLOCK_COUNT) {
            // File full so stop
            if (!SD.card()->writeStop()) {
                error("writeStop failed");
            }
            SD_CreateBinFile();
        }
    }
    else
        WTTRACE("SDCard busy\r\n");

    SD.card()->spiStop();
}

void dumpSdcard (bool isConsole)
{
    MYTRACE("filename:     %s\r\n", filename);
    MYTRACE("filepart:     %d\r\n", filepart);
    MYTRACE("blocknumber:  %d\r\n", bn);
}

void setupSdcard ()
{
    module_add(TAG);
    if (SD_Init() != 0) {
        Init_SetSemiFatal();
        TTRACE("init Failed!\r\n");
    } else {
        SD_CreateBinFile();
        TTRACE("init Done.\r\n");
        module_setup(TAG, FXTM_SUCCESS);
    }
    SD.card()->spiStop();
}

void loopSdcard ()
{
    SD_RecordBinFile();
}
