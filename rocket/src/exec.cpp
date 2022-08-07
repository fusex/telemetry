#define TAG "EXEC"

#include <SimpleTimer.h>
#include <fusexutil.h>

#define BGC_ACQ_PERIOD 100 // in millis on each second.

static SimpleTimer scheduler;
static bool        isPaused = false;

extern void subLoop();

void exec_pausedSet (bool paused)
{
    isPaused = paused;
}

void setupExec ()
{
    module_add(TAG);
    scheduler.setTimer(BGC_ACQ_PERIOD, subLoop, scheduler.RUN_FOREVER);
    TTRACE("init Done.\r\n");
    module_setup(TAG, FXTM_SUCCESS);
}

void loopExec ()
{
    if (!isPaused) {
        scheduler.run();
    }
}
