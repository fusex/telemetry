#include <Arduino.h>
#include <stdint.h>
#include "fusexconfig.h"
#include "fusexutil.h"

#ifdef CONFIG_PROFILING
static uint32_t prof_time;
static uint32_t prof_avg = 0;
static uint32_t prof_max = 0;
static uint32_t prof_count = 0;
#define PROF_LOOP 200UL

void prof_start() {
    prof_time = micros();
}

void prof_report()
{
    uint32_t delta = micros() - prof_time;
    if (delta>prof_max) prof_max = delta;
    prof_avg += delta;

    if (prof_count++%PROF_LOOP) {
	    TTRACE("%ld loop in:%lu (avg:%lu, max:%lu)\r\n", 
                prof_count, delta ,prof_avg/(prof_count+1), prof_max);
    }
}

#else

void prof_start() {}
void prof_report() {}

#endif