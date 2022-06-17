#include <Arduino.h>
#include <SimpleSerialShell.h>

#include "fusexutil.h"
#include "init.h"
#include "debug.h"
#include "version.h"
#include "trame.h"
#include "prof.h"
#include "rtc.h"

int showID (int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    shell.println(F(
		    "Built "   __DATE__ ":" __TIME__"\n\r"
		    "Version " VERSION  "\n\r"
		    "Running " __FILE__ "\n\r"
		    ));
    return 0;
}

int scanI2c(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    i2c_scanner(true);
    return 0;
}

int resetBoard(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    Init_ResetBoard();
    return 0;
}

int fxtmStatus(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    fxtm_dump(true);
    return 0;
}

int profStatus(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    prof_dump(true);
    return 0;
}

int rtcStatus(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    RTC_DumpDebug(true);
    return 0;
}

void setupShell()
{
    SHELLdevice.begin(SHELLSERIALBAUD);
    shell.attach(SHELLdevice);
    shell.addCommand(F("id"), showID);
    shell.addCommand(F("i2c-scan"), scanI2c);
    shell.addCommand(F("reset"), resetBoard);
    shell.addCommand(F("fxtm"), fxtmStatus);
    shell.addCommand(F("prof"), profStatus);
    shell.addCommand(F("rtc"), rtcStatus);
}

void loopShell()
{
    shell.executeIfInput();
}
