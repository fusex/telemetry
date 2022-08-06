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

static int showID (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    shell.println(F(
		    "Built "   __DATE__ ":" __TIME__"\n\r"
		    "Version " VERSION  "\n\r"
		    "Running " __FILE__ "\n\r"
		    ));
    return 0;
}

static int readI2c(int argc, char** argv)
{
    if (argc != 3) {
        shell.println("bad argument count");
        return -1;
    }

    uint8_t address = strtol(argv[1], NULL, 16);
    uint8_t reg = strtol(argv[2], NULL, 16);
    uint8_t val = i2c_read(true, address, reg);

    shell.println(val, HEX);

    return 0;
}

static int writeI2c(int argc, char** argv)
{
    if (argc != 4) {
        shell.println("bad argument count");
        return -1;
    }

    uint8_t address = strtol(argv[1], NULL, 16);
    uint8_t reg = strtol(argv[2], NULL, 16);
    uint8_t val = strtol(argv[3], NULL, 16);

    i2c_write(true, address, reg, val);

    return 0;
}

static int scanI2c(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    i2c_scanner(true);

    return 0;
}

static int resetBoard(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    Init_ResetBoard();

    return 0;
}

static int execModules(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    modules_printall(true);

    return 0;
}

static int fxtmStatus(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    fxtm_dump(true);

    return 0;
}

static int profStatus(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    prof_dump(true);

    return 0;
}

static int rtcStatus(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    RTC_DumpDebug(true);

    return 0;
}

static bool isPaused = false;

static int execPause(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    isPaused = true;

    return 0;
}

static int execResume(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    isPaused = false;

    return 0;
}

/* #########################################
                    Public
   ######################################### */

bool execIsPaused()
{
    return isPaused;
}

void setupShell()
{
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
}

void loopShell()
{
    shell.executeIfInput();
}
