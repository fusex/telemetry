#ifndef FUSEXUTIL_H
#define FUSEXUTIL_H

#include "fusexconfig.h"

# if defined(__AVR_ATmega2560__)
# define _FMT_FLASH_TYPE __FlashStringHelper
# define FMT(x)          F(x)
#else
# define _FMT_FLASH_TYPE char
# define  FMT(x)         (x)
#endif

#if !defined(_IS_HOST)
# include <Arduino.h>
#else
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <stdarg.h>
# include <math.h>
# include <sys/time.h>
#endif

#if 0
#if _IS_BASE
# include <SoftwareSerial.h>
extern SoftwareSerial SWSerial;
#endif
#endif

void dynTrace_enable(bool enable);
bool dynTrace_isEnable();

void _myprintf(const  _FMT_FLASH_TYPE *fmt, ... );
void _mycprintf(const  _FMT_FLASH_TYPE *fmt, ... );
unsigned long _mymillis();
long _myrandom(long min, long max);

int calcrc16(const uint8_t *ptr, uint16_t count);
uint16_t _gen_crc16(const uint8_t *data, uint16_t size);
void gendata(uint8_t* data, unsigned int size);
void fatal(void);

typedef uint8_t fxtm_error_t;
#define FXTM_SUCCESS 0
#define FXTM_FAILURE 1

void module_add(const char* tag);
void module_setup(const char* tag, fxtm_error_t error);
void modules_printall(bool isConsole);

#ifndef TAG
# define TAG "unknown-module"
#endif

#define myprintf(a, ...) _myprintf(FMT(a), ##__VA_ARGS__)

#if !defined(_IS_HOST)
# define mycprintf(a, ...) _mycprintf(FMT(a), ##__VA_ARGS__)
# define PRINT(x, ...)   DEBUGdevice.print(x, ##__VA_ARGS__)
# define PRINTS(x)       DEBUGdevice.print(x)
# define CPRINTS(x)      SHELLdevice.print(x)
# define PRINTLN(x, ...) DEBUGdevice.println(x, ##__VA_ARGS__)
# define PRINTMILLIS()   PRINT((float)_mymillis()/1000,6)
#else
# define PRINT(x, ...)   printf(x, ##__VA_ARGS__)
# define PRINTS(x)       printf("%s", x)
# define PRINTLN(x, ...) printf(x"\n", ##__VA_ARGS__)
# define PRINTMILLIS()   PRINT("%.6f",(float)_mymillis()/1000)
#endif

#define MYTRACE(x, ...) { \
    if (isConsole) { \
        CTRACE(x, ##__VA_ARGS__); \
    } else { \
        TRACE(x, ##__VA_ARGS__); \
    } \
}

#define MYTTRACE(x, ...) { \
    if (isConsole) { \
        CTTRACE(x, ##__VA_ARGS__); \
    } else { \
        TTRACE(x, ##__VA_ARGS__); \
    } \
}

#define DYNTTRACE(x, ...) { \
    if (dynTrace_isEnable()) { \
        CTTRACE(x, ##__VA_ARGS__); \
    } else { \
        TTRACE(x, ##__VA_ARGS__); \
    } \
}

#define CTRACE(x, ...) do { \
    mycprintf(":" x, ##__VA_ARGS__); \
} while(0)

#define TTRACE(x, ...) do { \
    PRINTMILLIS(); \
    myprintf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#define CTTRACE(x, ...) do { \
    PRINTMILLIS(); \
    mycprintf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#define TRACE(x, ...) myprintf(x, ##__VA_ARGS__)

#ifdef CONFIG_DEBUG
# define DTRACE(x, ...)  TRACE(x, ##__VA_ARGS__)
# define DTTRACE(x, ...) TTRACE(x, ##__VA_ARGS__)
void printdata(char* data, unsigned int size);
#else
# define DTRACE(x, ...)  do {} while(0)
# define DTTRACE(x, ...) do {} while(0)
#endif

#ifdef CONFIG_TRACES
# define WTRACE(x, ...)  TRACE(x, ##__VA_ARGS__)
# define WTTRACE(x, ...) TTRACE(x, ##__VA_ARGS__)
void printdata(char* data, unsigned int size);
#else
# define WTRACE(x, ...) do {} while(0)
# define WTTRACE(x, ...) do {} while(0)
#endif

#define PCSERIALBAUD     115200
#define DEBUGSERIALBAUD  115200
#define SHELLSERIALBAUD  115200
#define GPSSERIALBAUD      9600

#endif // FUSEXUTIL_H
