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

#include <thread>
#include <mutex>
#include <unistd.h>

#include <condition_variable>

#include <sys/time.h>

#define MAX_FILE_PATH 128
#define SLOT_MAX       64

#define HDR_SIZE  32
#define MSG_SIZE 476

#if 0
#define DEBUG
#endif

#define trace(f,...) printf(f , ##__VA_ARGS__ )

#ifdef DEBUG
#define dtrace(f,...) printf(f , ##__VA_ARGS__ )
#else
#define dtrace(f, ...) do {} while(0);
#endif

#define trace(f,...) printf(f , ##__VA_ARGS__ )

typedef struct {
  char     header[HDR_SIZE];
  char     logmsg[MSG_SIZE];
  uint32_t id;
} log;
#define BUILD_BUG_ON (sizeof(log)%512);

#define PERIODIC_SYNC 2

class logger {
    std::thread             task;
    std::thread             periodicT;
    std::mutex              mLock;
    std::condition_variable processIt;
    bool                    notified;
    bool                    mustDied = false;
    FILE*                   logfile;
    char                    logfilename[MAX_FILE_PATH];
    log*                    cloglist;
    
    volatile uint32_t p;
    volatile uint32_t c;
    volatile bool full;
    uint32_t gcount = 0;
    uint64_t ids = 0;
    uint64_t readp = 0;

    void logthread(void);
    long long gettimestamp();
    void logfilewriter();
    void periodicthread();
    void flush();

public:
    logger(const char*);

    ~logger();

    void hup(void);
    void lprintf(const char* fmt, ...);
    void rlog();
    void wlog(char* buf, size_t size);
};
