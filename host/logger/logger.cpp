#include <assert.h>
#include <string.h> 
#include <stdarg.h> 

#include "logger.h"

#define INC_P() { \
    p.fetch_add(1, std::memory_order_relaxed); \
    std::atomic_thread_fence(std::memory_order_release); \
    if ((p - x.load(std::memory_order_relaxed)) == SLOT_MAX) { \
        dtrace("ring full\n"); \
        full = true; \
        hup(); \
        while (full); \
        dtrace("ring not more full\n"); \
    }; \
    std::atomic_thread_fence(std::memory_order_release); \
    if (p - x.load(std::memory_order_release) == 1) { \
        dtrace("send hup1\n"); \
        hup(); \
    } \
    debug1("P"); \
}

#define INC_C() { \
    c.fetch_add(1, std::memory_order_relaxed); \
    std::atomic_thread_fence(std::memory_order_release); \
    debug1("C"); \
}

#define INC_X() { \
    x.fetch_add(1, std::memory_order_relaxed); \
    std::atomic_thread_fence(std::memory_order_release); \
    debug1("X"); \
}

#define myassert(cond) { \
    std::atomic_thread_fence(std::memory_order_release); \
    if (!(cond)) {\
        debug("asserting at"); \
    } \
    assert(cond); \
}

void logger::logthread()
{
    while (!mustDie) {
        std::unique_lock<std::mutex> locker(mLock);
        std::atomic_thread_fence(std::memory_order_release);
        while (x == p.load(std::memory_order_release)) {
            //debug("logthread wait for hup at");
            processIt.wait(locker);
            //debug("logthread get a hup at");
            std::atomic_thread_fence(std::memory_order_release);
        }
        logfilewriter();
    }
}

void logger::debug1(const char* h)
{
    std::atomic_thread_fence(std::memory_order_release);
    trace("%s (D1) ring status p:%d/x:%d/c:%d\n", h,
            p.load(std::memory_order_relaxed),
            x.load(std::memory_order_relaxed),
            c.load(std::memory_order_relaxed));
}


void logger::debug(const char* h)
{
    std::atomic_thread_fence(std::memory_order_release);
    trace("%s ring status p:%d/x:%d/c:%d\n", h,
            p.load(std::memory_order_relaxed),
            x.load(std::memory_order_relaxed),
            c.load(std::memory_order_relaxed));
}

#define hexdump(buf, len) do { \
    unsigned char *p; \
    uint32_t s = len; \
    for (p = (unsigned char*) buf; s-- > 0; p++) \
            printf(" 0x%x", *p); \
    printf("\n"); \
} while (0)

void logger::logfilewriter()
{
   myassert(p.load(std::memory_order_release) - x <= SLOT_MAX);

    debug("logwriter");
    bool do_fflush = false;
    std::atomic_thread_fence(std::memory_order_release);
    while (p.load(std::memory_order_release) - x) {
        uint32_t slotId = x%SLOT_MAX;
        fwrite(&cloglist[slotId], FXLOG_SIZE, 1, logfile);
        INC_X();
        do_fflush = true;
        std::atomic_thread_fence(std::memory_order_release);
    }

    if (full) full = false;

    if (do_fflush) {
        fflush(logfile);
    }
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
   myassert(abs(p - x.load(std::memory_order_release)) < SLOT_MAX);

   uint32_t slotId = p%SLOT_MAX;

   sprintf(cloglist[slotId].header, "  [%20lld]: ", gettimestamp());
   cloglist[slotId].id = ids++;

   va_start(ap,fmt);
   vsprintf(cloglist[slotId].logmsg, fmt, ap);
   va_end(ap);

   INC_P();
}

void logger::wlog(uint8_t* buf, size_t size)
{
   myassert(p - x.load(std::memory_order_release) <= SLOT_MAX);
   assert(size < MSG_SIZE);

   uint32_t slotId = p%SLOT_MAX;

   sprintf(cloglist[slotId].header, "  [%20lld]: ", gettimestamp());
   cloglist[slotId].id = ids++;

   memcpy(cloglist[slotId].logmsg, buf, FXLOG_SIZE);

   INC_P();
}

size_t logger::rlog(uint8_t* buf, size_t size)
{
    size_t s = size;  
 
    std::atomic_thread_fence(std::memory_order_release);
    while (p == c.load(std::memory_order_release)) {
        std::unique_lock<std::mutex> locker(mLock);
        //debug("rlog wait for hup at");
        processIt.wait(locker);
        //debug("rlog get a hup at");
        std::atomic_thread_fence(std::memory_order_release);
    }

    std::atomic_thread_fence(std::memory_order_release);
    //check if it is not too late to read from ring-buffer.
    if ((p.load(std::memory_order_release) - c) < SLOT_MAX) {
        uint32_t slotId = c%SLOT_MAX;
        memcpy(buf, cloglist[slotId].logmsg, size);
        s = size;
    }

    //check again in case we have race condition, if yes,
    //re-read from logger file.
    if ((p.load(std::memory_order_release) - c) >= SLOT_MAX) {
        int err = fseek(readfile, c*FXLOG_SIZE, SEEK_SET); //TODO: we should check c overflow
        myassert(err==0);
        s = fread(buf, size, 1, readfile);
        s *= size;
    }

    INC_C();

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
