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

#if 1
#define LOG_SAME
#endif

#define INC_P() { \
    p.fetch_add(1, std::memory_order_relaxed); \
    std::atomic_thread_fence(std::memory_order_release); \
    if ((p.load(std::memory_order_relaxed) - c.load(std::memory_order_relaxed)) == SLOT_MAX) { \
	dtrace("ring full\n"); \
	full = true; \
	hup(); \
	while(full); \
	dtrace("ring not more full\n"); \
    }; \
    std::atomic_thread_fence(std::memory_order_release); \
    if ( \
        ((p.load(std::memory_order_release) - c.load(std::memory_order_release)) == 1) || \
        ((p.load(std::memory_order_release) - x.load(std::memory_order_release)) == 1)) \
    { \
	dtrace("send hup1\n"); \
	hup(); \
    } \
    debug("P"); \
}

#define INC_C() { \
    c.fetch_add(1, std::memory_order_relaxed); \
    std::atomic_thread_fence(std::memory_order_release); \
    debug("C"); \
}

#define INC_X() { \
    x.fetch_add(1, std::memory_order_relaxed); \
    std::atomic_thread_fence(std::memory_order_release); \
    debug("X"); \
}

#define myassert(cond) { \
    std::atomic_thread_fence(std::memory_order_release); \
    if(!(cond)) \
        debug("asserting at"); \
    assert(cond); \
}

void logger::logthread()
{
    while(!mustDie) {
	std::unique_lock<std::mutex> locker(mLock);
	std::atomic_thread_fence(std::memory_order_release);
	while(c.load(std::memory_order_release) == p.load(std::memory_order_release)) {
	    debug("logthread wait for hup at");
	    processIt.wait(locker);
	    debug("logthread get a hup at");
	    std::atomic_thread_fence(std::memory_order_release);
	}
	logfilewriter();
    }
}

void logger::debug(const char* h)
{
    std::atomic_thread_fence(std::memory_order_release);
    trace("%s ring status p:%d/c:%d/x:%d\n", h, 
	    p.load(std::memory_order_relaxed),
            c.load(std::memory_order_relaxed),
            x.load(std::memory_order_relaxed));
}

void logger::logfilewriter()
{
    debug("logwriter");
    bool do_fflush = false;
    std::atomic_thread_fence(std::memory_order_release);
    while(p.load(std::memory_order_release) - c.load(std::memory_order_release)) {
        fwrite(&cloglist[c], 512, 1, logfile);
        INC_C();
	do_fflush = true;
	std::atomic_thread_fence(std::memory_order_release);
    }

    if(full) full = false;

    if(do_fflush) {
	fflush(logfile);
    }
    std::atomic_thread_fence(std::memory_order_release);
    //ddebug("vfsynced");
}

void logger::flush()
{
    dtrace("final synchro\n");
    sleep(1);
    hup();
}

long long logger::gettimestamp()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);

    return tp.tv_sec * 1000000 + tp.tv_usec;
}

void logger::hup()
{
    processIt.notify_all(); 
}

void logger::lprintf(const char* fmt, ...)
{
   va_list ap;
   myassert(abs(p - c) < SLOT_MAX);

#ifndef LOG_SAME 
   sprintf(cloglist[p.load(std::memory_order_release)].header, "  [%20lld]: ", gettimestamp());
   cloglist[p.load(std::memory_order_release)].id = ids++;
#endif

   va_start(ap,fmt);
   vsprintf(cloglist[p.load(std::memory_order_release)].logmsg, fmt, ap);
   va_end(ap);

   INC_P();
}

void logger::wlog(uint8_t* buf, size_t size)
{
   myassert(abs(p - c) < SLOT_MAX);
   assert(size < MSG_SIZE);

#ifndef LOG_SAME 
   sprintf(cloglist[p.load(std::memory_order_release)].header, "  [%20lld]: ", gettimestamp());
   cloglist[p.load(std::memory_order_release)].id = ids++;
#endif

   memcpy(cloglist[p.load(std::memory_order_release)].logmsg, buf, size);

   INC_P();
}

size_t logger::rlog(uint8_t* buf, size_t size)
{
    size_t s = size;  
 
    myassert(x<=p);

    std::atomic_thread_fence(std::memory_order_release);
    while(x.load(std::memory_order_release) == p.load(std::memory_order_release)) {
	std::unique_lock<std::mutex> locker(mLock);
	debug("rlog wait for hup at");
	processIt.wait(locker);
	debug("rlog get a hup at");
	std::atomic_thread_fence(std::memory_order_release);
    }

    std::atomic_thread_fence(std::memory_order_release);
    if(p.load(std::memory_order_release) - x.load(std::memory_order_release)> SLOT_MAX) {
	int err= fseek(readfile, x.load(std::memory_order_release)*512, SEEK_SET);
	myassert(err==0);
	s = fread(buf, size, 1, readfile); //TODO check this
	s *= size;
    } else
	memcpy(buf, cloglist[x.load(std::memory_order_release)%SLOT_MAX].logmsg, size);

    INC_X();

    return s;
}

void logger::join()
{
    task.join();
}

logger::~logger()
{
    dtrace("calling dtor\n");
    mustDie = true;
    fclose(logfile);
    fclose(readfile);
    free(cloglist);
}

logger::logger(const char* filename)
{
    assert(strlen(filename) < MAX_FILE_PATH);
    strcpy(logfilename,filename);

    logfile = fopen(logfilename,"w+");
    assert(logfile);

    cloglist = (fxlog*) malloc(sizeof(fxlog)*SLOT_MAX);
    assert(cloglist);

    readfile = fopen(logfilename,"ro");
    assert(readfile);

    task = std::thread(&logger::logthread,this);
    //task.detach();
}
