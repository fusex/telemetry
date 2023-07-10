#define TAG "SHELL"

#include <Arduino.h>
#include <SimpleSerialShell.h>
#include <fusexutil.h>
#include <trame.h>
#include <stdlib.h>

#include "init.h"
#include "debug.h"
#include "version.h"
#include "prof.h"
#include "rtc.h"
#include "exec.h"
#include "sdcard.h"
#include "gps.h"
#include "flash.h"
#include "atmos.h"
#include "imu.h"

static int showID (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    _SHELLTRACE("Built "   __DATE__ ":" __TIME__"\n\r"
            "Version " VERSION  "\n\r"
            "Running " __FILE__ "\n\r"
    );
    return 0;
}

static int readI2c (int argc, char** argv)
{
    if (argc != 3) {
        _SHELLTRACE("bad argument count\n\r");
        return -1;
    }

    uint8_t adr = strtol(argv[1], NULL, 16);
    uint8_t reg = strtol(argv[2], NULL, 16);
    uint8_t val = i2c_read(true, adr, reg);

    SHELLPRINTS(val, HEX);

    return 0;
}

static int writeI2c (int argc, char** argv)
{
    if (argc != 4) {
        _SHELLTRACE("bad argument count");
        return -1;
    }

    uint8_t adr = strtol(argv[1], NULL, 16);
    uint8_t reg = strtol(argv[2], NULL, 16);
    uint8_t val = strtol(argv[3], NULL, 16);

    i2c_write(true, adr, reg, val);

    return 0;
}

static int scanI2c (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    i2c_scanner(true);

    return 0;
}

static int resetBoard (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    Init_ResetBoard();

    return 0;
}

static int execModules (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    modules_printall(true);

    return 0;
}

static int fxtmStatus (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    size_t len = fxtm_dumpdata(NULL, consolebuf, CONSOLE_BUF_SIZE);
    SHELLPRINTS(consolebuf);

    _SHELLTRACE("fxtm string (serial) size:%3d Bytes\n\r", len);
    _SHELLTRACE("fxtm txdata (flash)  size:%3d Bytes\n\r", fxtm_gettxdatasize());
    _SHELLTRACE("fxtm block  (sdcard) size:%3d Bytes\n\r", fxtm_getblocksize());
    _SHELLTRACE("fxtm data   (radio)  size:%3d Bytes\n\r", fxtm_getdatasize());

    return 0;
}

static int profStatus (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    prof_dump(true);

    return 0;
}

static int rtcStatus (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    RTC_DumpDebug(true);

    return 0;
}

static int execPause (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    exec_pausedSet(true);

    return 0;
}

static int execResume (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    exec_pausedSet(false);

    return 0;
}

static int dynTraceOn (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dynTrace_enable(true);

    return 0;
}

static int dynTraceOff (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dynTrace_enable(false);

    return 0;
}

static int sdcardStatus (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dumpSdcard(true);

    return 0;
}

static int gpsStatus (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dumpGPS(true);

    return 0;
}

static int flashDump (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dumpFlash(true);

    return 0;
}

static int atmosDump (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dumpAtmos(true);

    return 0;
}

static int imuDump (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    dumpIMU(true);

    return 0;
}

static int flashRead (int argc, char** argv)
{
    uint32_t addr = 0;

    if (argc > 0) {
        addr = strtoul(argv[1], NULL, 16);
    }

    _SHELLTRACE("Flash page read from:0x%lx:\n\r", addr);
    readFlash(true, addr);

    return 0;
}

static int flashWrite (int argc, char** argv)
{
    uint32_t addr = 0;
    uint32_t val = 0;

    if (argc > 1) {
        addr = strtoul(argv[1], NULL, 16);
        val  = strtoul(argv[2], NULL, 16);
    }

    _SHELLTRACE("Flash write u32 value:0x%lx at:0x%lx:\n\r", val, addr);
    writeFlash(addr, val);

    return 0;
}

static int flashSlice_setup (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    _SHELLTRACE(" Start Flash setup of Slices ... ");
    setupFlashSlice();
    _SHELLTRACE(" done\n\r");

    return 0;
}

static int flashErase (int argc, char** argv)
{
    uint32_t addr = 0;

    if (argc > 0) {
        addr = strtoul(argv[1], NULL, 16);
    }

    _SHELLTRACE("Flash erase chip ...", addr);
    eraseFlash(addr);
    _SHELLTRACE(" done\n\r");
    _SHELLTRACE("Warning: It's highly recommanded to reset the board "
                "(throught reset button or reset shell command) to "
                "re-initialize flash slice and pointers!\r\n");

    return 0;
}

/* #########################################
                    Public
   ######################################### */

void setupShell ()
{
    module_add(TAG);

    SHELLdevice.begin(SHELLSERIALBAUD);
    shell.attach(SHELLdevice);
    shell.addCommand(F("id"), showID);
    shell.addCommand(F("i2c-scan"), scanI2c);
    shell.addCommand(F("i2c-read"), readI2c);
    shell.addCommand(F("i2c-write"), writeI2c);
    shell.addCommand(F("reset"), resetBoard);
    shell.addCommand(F("fxtm"), fxtmStatus);
    shell.addCommand(F("prof"), profStatus);
    shell.addCommand(F("rtc"), rtcStatus);
    shell.addCommand(F("pause"), execPause);
    shell.addCommand(F("resume"), execResume);
    shell.addCommand(F("modules"), execModules);
    shell.addCommand(F("trace"), dynTraceOn);
    shell.addCommand(F("notrace"), dynTraceOff);
    shell.addCommand(F("sdcard"), sdcardStatus);
    shell.addCommand(F("gps"), gpsStatus);
    shell.addCommand(F("flash"), flashDump);
    shell.addCommand(F("flash-read"), flashRead);
    shell.addCommand(F("flash-write"), flashWrite);
    shell.addCommand(F("flash-erase"), flashErase);
    shell.addCommand(F("flash-slice-setup"), flashSlice_setup);
    shell.addCommand(F("atmos"), atmosDump);
    shell.addCommand(F("imu"), imuDump);

    module_setup(TAG, FXTM_SUCCESS);
}

void loopShell ()
{
    shell.executeIfInput();
}
