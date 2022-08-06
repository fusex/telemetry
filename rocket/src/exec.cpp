#define TAG "EXEC"

#include <SimpleTimer.h>

#define BGC_ACQ_PERIOD 100 // in millis on each second.

SimpleTimer scheduler;

static bool isPaused = false;

extern void subLoop();

bool execIsPaused()
{
    return isPaused;
}

void execSetPaused(bool paused)
{
    isPaused = paused;
}

void setupExec()
{
    scheduler.setTimer(BGC_ACQ_PERIOD, subLoop, scheduler.RUN_FOREVER);
}

void loopExec()
{
    if (!execIsPaused()) {
        scheduler.run();
    }
}

