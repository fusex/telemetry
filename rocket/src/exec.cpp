#define TAG "EXEC"

#include <SimpleTimer.h>

#define BGC_ACQ_PERIOD 100 // in millis on each second.

static SimpleTimer scheduler;

static bool isPaused = false;

extern void subLoop();

void exec_pausedSet(bool paused)
{
    isPaused = paused;
}

void setupExec()
{
    scheduler.setTimer(BGC_ACQ_PERIOD, subLoop, scheduler.RUN_FOREVER);
}

void loopExec()
{
    if (!isPaused) {
        scheduler.run();
    }
}

