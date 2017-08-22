/*
* =====================================================================================
*
*       Filename:  logger.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  05/15/2017 11:39:11 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@harman.com
*        Company:  Harman
*
* =====================================================================================
*/

#include <assert.h>
#include <string.h> 
#include <stdarg.h> 
#include "logger.h"

#define INC_P() { \
    dtrace("message id:%4d pushed into ring buffer\n", SLOT_MAX*gcount+p);  \
    p++; \
    if ((p - c) == SLOT_MAX) flush(); \
    if (p == SLOT_MAX) { p = 0 ; gcount++; } \
    dtrace("ring status %d/%d\n", c,p); \
}

#define INC_C() { \
    dtrace("%d/%d\n", c, p); \
    c++; \
    if (c == SLOT_MAX) {c = 0;} \
}

#define myassert(cond) { \
    if(!(cond)) \
        trace("asserting at id:%d p:%d c:%d\n", SLOT_MAX*gcount+p ,p,c); \
    assert(cond); \
}

void logger::periodicthread()
{
    while(!mustDied) {
        sleep(PERIODIC_SYNC);
        hup();
    }
}

void logger::logthread()
{
    while(!mustDied) {
        std::unique_lock<std::mutex> locker(mLock);
         while(!notified) {
             processIt.wait(locker);
         }
         dtrace("get a hup\n");
         logfilewriter();
         notified = false;
    }
}

void logger::logfilewriter()
{
    dtrace("writing logs %d/%d\n", c, p);
    while(p-c) {
        fwrite(&cloglist[c], 512, 1, logfile);
        INC_C();
    }
    dtrace("vfsynced %d/%d\n", c, p);
    full = false;
}

long long logger::gettimestamp()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);

    return tp.tv_sec * 1000000 + tp.tv_usec;
}

void logger::hup()
{
    notified = true;
    processIt.notify_one(); 
}


void logger::flush()
{
    dtrace("flush ing\n");
    full = true;
    hup();
    while(full);
    dtrace("finished\n");
}

void logger::lprintf(const char* fmt, ...)
{
   va_list ap;
   myassert(abs(p - c) < SLOT_MAX);

   sprintf(cloglist[p].header, "  [%20lld]: ", gettimestamp());

   va_start(ap,fmt);
   vsprintf(cloglist[p].logmsg, fmt, ap);
   va_end(ap);
   cloglist[p].id = ids++;

   INC_P();
}

void logger::wlog(char* buf, size_t size)
{
   myassert(abs(p - c) < SLOT_MAX);
   assert(size < MSG_SIZE);
   sprintf(cloglist[p].header, "  [%20lld]: ", gettimestamp());
   memcpy(cloglist[p].logmsg, buf, size);
   cloglist[p].id = ids++;

   INC_P();
}

size_t logger::rlog(char* buf, size_t size)
{
    size_t s = size;  
    if((SLOT_MAX*gcount)<readp)
	memcpy(buf, cloglist[p].logmsg, size);
    else
	s = fread(buf, size, 1, readfile);

    readp++;

    return s;
}

logger::~logger()
{
    dtrace("calling dtor\n");
    mustDied = true;
    flush();
    fclose(logfile);
    free(cloglist);
}

logger::logger(const char* filename)
{
    assert(strlen(filename) < MAX_FILE_PATH);
    strcpy(logfilename,filename);

    logfile = fopen(logfilename,"w+");
    assert(logfile);

    cloglist = (log*) malloc(sizeof(log)*SLOT_MAX);
    assert(cloglist);

    readfile = fopen(logfilename,"ro");
    assert(readfile);

    task = std::thread(&logger::logthread,this);
    task.detach();

    periodicT = std::thread(&logger::periodicthread,this);
    periodicT.detach();
}
