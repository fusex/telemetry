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
    ddtrace("message id:%4d pushed into ring buffer\n", SLOT_MAX*gcount+p); \
    p++; \
    hup(); \
    if ((p - c) == SLOT_MAX) { \
	dtrace("ring full\n"); \
	full = true; \
	while(full); \
	dtrace("ring not more full\n"); \
    }; \
    if (p == SLOT_MAX) { p = 0 ; gcount++; } \
    dtrace("P ring status p:%d/c:%d/readp:%ld/id:%d\n",p,c,readp,SLOT_MAX*gcount+p); \
}

#define INC_C() { \
    c++; \
    if (c == SLOT_MAX) {full = false; c = 0;} \
    dtrace("C ring status p:%d/c:%d/readp:%ld/id:%d\n",p,c,readp,SLOT_MAX*gcount+p); \
}

#define INC_READP() { \
    readp++; \
    dtrace("READP ring status p:%d/c:%d/readp:%ld/id:%d\n",p,c,readp,SLOT_MAX*gcount+p); \
}

#define myassert(cond) { \
    if(!(cond)) \
        trace("asserting at p:%d/c:%d/readp:%ld/id:%d\n",p,c,readp,SLOT_MAX*gcount+p); \
    assert(cond); \
}

void logger::logthread()
{
    while(!mustDied) {
	std::unique_lock<std::mutex> locker(mLock);
	processIt.wait(locker);
	ddtrace("get a hup\n");
	logfilewriter();
    }
}

void logger::logfilewriter()
{
    bool do_fflush = false;
    ddtrace("writing logs %d/%d\n", c, p);
    while(p-c) {
        fwrite(&cloglist[c], 512, 1, logfile);
        INC_C();
	do_fflush = true;
    }

    if(do_fflush) {
	fflush(logfile);
    }
    ddtrace("vfsynced p:%d/c:%d/readp:%ld\n", c, p, readp);
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

   sprintf(cloglist[p].header, "  [%20lld]: ", gettimestamp());

   va_start(ap,fmt);
   vsprintf(cloglist[p].logmsg, fmt, ap);
   va_end(ap);
   cloglist[p].id = ids++;

   INC_P();
}

void logger::wlog(uint8_t* buf, size_t size)
{
   myassert(abs(p - c) < SLOT_MAX);
   assert(size < MSG_SIZE);
   sprintf(cloglist[p].header, "  [%20lld]: ", gettimestamp());
   memcpy(cloglist[p].logmsg, buf, size);
   cloglist[p].id = ids++;

   INC_P();
}

size_t logger::rlog(uint8_t* buf, size_t size)
{
    size_t s = size;  
 
    myassert(readp<=(SLOT_MAX*gcount+p));

    while(readp == (SLOT_MAX*gcount+p)) {
	std::unique_lock<std::mutex> locker(mLock);
	processIt.wait(locker);
    }

    if((SLOT_MAX*gcount)>readp)
    {
	int err= fseek(readfile, (readp*512)+(HDR_SIZE), SEEK_SET);
	myassert(err==0);
	s = fread(buf, size, 1, readfile); //TODO check this
	s *= size;
    } else
	memcpy(buf, cloglist[readp%SLOT_MAX].logmsg, size);

    INC_READP();

    return s;
}

logger::~logger()
{
    dtrace("calling dtor\n");
    mustDied = true;
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
    task.detach();
}
