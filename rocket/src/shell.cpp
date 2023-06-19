#define TAG "SHELL"

#include <Arduino.h>
#include <SimpleSerialShell.h>
#include <fusexutil.h>
#include <trame.h>

#include "init.h"
#include "debug.h"
#include "version.h"
#include "prof.h"
#include "rtc.h"
#include "exec.h"
#include "sdcard.h"
#include "gps.h"

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

    _SHELLTRACE("fxtm string size:%3d Bytes\n\r", len);
    _SHELLTRACE("fxtm block  size:%3d Bytes\n\r", fxtm_getblocksize());
    _SHELLTRACE("fxtm data   size:%3d Bytes\n\r", fxtm_getdatasize());

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

    TTRACE("init Done.\r\n");
    module_setup(TAG, FXTM_SUCCESS);
}

void loopShell ()
{
    shell.executeIfInput();
}
