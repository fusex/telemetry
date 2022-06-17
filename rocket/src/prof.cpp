#define TAG "PROF"

#include <Arduino.h>
#include <stdint.h>
#include "fusexconfig.h"
#include "fusexutil.h"

#ifdef CONFIG_PROFILING
static uint32_t prof_time;
static uint32_t prof_avg = 0;
static uint32_t prof_max = 0;
#define PROF_LOOP 200UL

void prof_start() {
    prof_time = micros();
}

void prof_dump (bool isConsole) {
   MYTRACE("prof_avg:   %ld\r\n", prof_avg);
   MYTRACE("prof_max:   %ld\r\n", prof_max);
}

void prof_report()
{
    uint32_t delta = micros() - prof_time;
    if (delta>prof_max) prof_max = delta;
    uint32_t prof_cumul = (9 * prof_avg) + delta;
    prof_avg = prof_cumul/10;

    TTRACE("Loop in:%lu (avg:%lu, max:%lu)\r\n", 
            delta ,prof_avg, prof_max);
}

#else

void prof_start() {}
void prof_report() {}
void prof_dump (isConsole);

#endif
