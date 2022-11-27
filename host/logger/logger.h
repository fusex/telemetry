#ifndef _LOGGER_H
#define _LOGGER_H

#include <thread>
#include <mutex>
#include <unistd.h>

#include <condition_variable>
#include <atomic>

#include <sys/time.h>

#if 0
#define DEBUG 1
#endif

#if 0
#define trace(f,...) printf(f , ##__VA_ARGS__ )
#else
#define trace(f,...) do {} while(0);
#endif

#ifdef DEBUG
#define dtrace(f,...) printf(f , ##__VA_ARGS__ )
#define ddtrace(f,...) { if(DEBUG>1) printf(f , ##__VA_ARGS__ );}
#else
#define dtrace(f, ...) do {} while(0);
#define ddtrace dtrace
#endif

#define MAX_FILE_PATH 128
#define SLOT_MAX       64
//#define SLOT_MAX       2

#define HDR_SIZE  32
#define MSG_SIZE 476

typedef struct {
  char     logmsg[MSG_SIZE];
  char     header[HDR_SIZE];
  uint32_t id;
} fxlog;
#define FXLOG_SIZE   sizeof(fxlog)
#define BUILD_BUG_ON (FXLOG_SIZE%512);

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
    void debug1(const char*);
    void flush();
    void join();
};
#endif // _LOGGER_H
