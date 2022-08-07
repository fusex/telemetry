#define TAG "PROF"

#include <Arduino.h>
#include <stdint.h>
#include "fusexconfig.h"
#include "fusexutil.h"

#ifdef CONFIG_PROFILING
static uint32_t prof_time;
static uint32_t prof_avg = 0;
static uint32_t prof_max = 0;
static uint32_t prof_last = 0;
#define PROF_LOOP 200UL

#if 0
# define DEBUG
#endif

void prof_start ()
{
    prof_time = micros();
}

void prof_dump (bool isConsole)
{
   MYTRACE("prof_last:  %ld\r\n", prof_last);
   MYTRACE("prof_avg:   %ld\r\n", prof_avg);
   MYTRACE("prof_max:   %ld\r\n", prof_max);
}

void prof_report ()
{
    prof_last = micros() - prof_time;
    uint32_t prof_cumul = (9 * prof_avg) + prof_last;

    if (prof_last>prof_max) prof_max = prof_last;
    prof_avg = prof_cumul/10;

#if DEBUG
    TTRACE("Loop in:%lu (avg:%lu, max:%lu)\r\n", 
            prof_last, prof_avg, prof_max);
#endif
}

#else

void prof_start(){}
void prof_report(){}
void prof_dump(bool isConsole){}

#endif
