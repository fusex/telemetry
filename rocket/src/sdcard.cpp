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
#include "flash.h"
#include "sdcard.h"

#define LOGFILENAME "fusexlog"
#define error(msg) {SD.errorPrint(&Serial, F(msg));}
#define FILE_BLOCK_COUNT (120*60*10L) // 120 minutes logging at 10HZ

static SdFat32      SD;
static File32       binFile;

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

    if (!binFile.open(filename, O_RDWR | O_CREAT)) {
        error("open failed");
    }

    if (!binFile.preAllocate(512UL * FILE_BLOCK_COUNT)) {
        error("preAllocate failed");
    }

    bn = 0;
}

static void SD_CheckFlashFile ()
{
    if (SD.exists("NOFLASH")) {
	skipFlash();
    }
}

#define SPI_CLOCK SD_SCK_MHZ(8)
#define SD_CONFIG SdSpiConfig(BGC_SD_CS, SHARED_SPI, SPI_CLOCK)

static int SD_Init ()
{
    if (!SD.begin(SD_CONFIG))
        return -1;

    return 0;
}

#define MAX_RETRY 100 

static int SD_RecordBinFile ()
{
    fxtm_block_t *pBlock = fxtm_getblock();
    int ret = 0;

    // Write block to SD.
    if (!binFile.write((uint8_t *)pBlock, fxtm_getblocksize())) {
        //error("write data failed");
        ret = -1;
    }

    if (ret == 0) {
        bn++;
        if (bn == FILE_BLOCK_COUNT) {
            // File full so create new file.
            SD_CreateBinFile();
        }
        int retry = 0;
        while (retry<MAX_RETRY) {
            if (!SD.card()->isBusy())
                break;
            delay(1);
            retry++;
        }
    }

    return ret;
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
        module_setup(TAG, FXTM_FAILURE);
        Init_SetSemiFatal();
    } else {
        SD_CreateBinFile();
        SD_CheckFlashFile();
        module_setup(TAG, FXTM_SUCCESS);
    }
}

void loopSdcard ()
{
    if (0 != SD_RecordBinFile()) {
        fxtm_seterror(FXTM_SDCARD);
    }
}
