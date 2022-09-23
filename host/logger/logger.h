/*
* =====================================================================================
*
*       Filename:  logger.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  05/15/2017 11:39:45 AM
*       Revision:  none
*       Compiler:  gcc
*
*
* =====================================================================================
*/

#ifndef _LOGGER_H
#define _LOGGER_H

#include <thread>
#include <mutex>
#include <unistd.h>

#include <condition_variable>
#include <atomic>

#include <sys/time.h>

#define MAX_FILE_PATH 128
#define SLOT_MAX       64

#define HDR_SIZE  32
#define MSG_SIZE 476

#if 1
#define DEBUG 1
#endif

#define trace(f,...) printf(f , ##__VA_ARGS__ )

#ifdef DEBUG
#define dtrace(f,...) printf(f , ##__VA_ARGS__ )
#define ddtrace(f,...) { if(DEBUG>1) printf(f , ##__VA_ARGS__ );}
#else
#define dtrace(f, ...) do {} while(0);
#define ddtrace dtrace
#endif

typedef struct {
  char     logmsg[MSG_SIZE];
  char     header[HDR_SIZE];
  uint32_t id;
} fxlog;
#define BUILD_BUG_ON (sizeof(fxlog)%512);

class logger {
    std::thread             task;
    std::mutex              mLock;
    std::condition_variable processIt;

    bool                    mustDie = false;
    FILE*                   logfile;
    FILE*                   readfile;
    char                    logfilename[MAX_FILE_PATH];
    fxlog*                  cloglist;

    std::atomic<uint32_t> p{0};
    std::atomic<uint32_t> c{0};
    std::atomic<uint32_t> x{0};

    volatile bool full;
    uint64_t ids = 0;

    void logthread(void);
    long long gettimestamp();
    void logfilewriter();

public:
    logger(const char*);

    ~logger();

    void hup(void);
    void lprintf(const char* fmt, ...);
    size_t rlog(uint8_t* buf, size_t size);
    void wlog(uint8_t* buf, size_t size);
    void debug(const char*);
    void flush();
    void join();
};
#endif // _LOGGER_H
