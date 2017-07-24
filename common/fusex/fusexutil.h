#ifndef FUSEXUTIL_H
#define FUSEXUTIL_H

#include "fusexconfig.h"

#if !defined(_IS_PC)
# include <Arduino.h>
# define _FMT_FLASH_TYPE __FlashStringHelper
#else
# define _FMT_FLASH_TYPE char
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h> 
#include <sys/time.h>
#endif

#if _IS_BASE
#include <SoftwareSerial.h>
extern SoftwareSerial SWSerial;
#endif

void _myprintf(const  _FMT_FLASH_TYPE *fmt, ... );
unsigned long _mymillis();
long _myrandom(long min, long max);

int calcrc16(const uint8_t *ptr, uint16_t count);
uint16_t _gen_crc16(const uint8_t *data, uint16_t size);
void gendata(uint8_t* data, unsigned int size);
void fatal(void);

#ifndef TAG
# define TAG "unknown-module"
#endif

#if !defined(_IS_PC)
# define myprintf(a, ...) _myprintf(F(a), ##__VA_ARGS__)
# define PRINT(x, ...)   DEBUGdevice.print(x, ##__VA_ARGS__)
# define PRINTS(x) DEBUGdevice.print(x)
# define PRINTLN(x, ...) DEBUGdevice.println(x, ##__VA_ARGS__)
#define  PRINTMILLIS() PRINT((float)_mymillis()/1000,6)
#else
# define myprintf(a, ...) _myprintf(a, ##__VA_ARGS__)
# define PRINT(x, ...)   printf(x, ##__VA_ARGS__)
# define PRINTS(x)   printf("%s", x)
# define PRINTLN(x, ...) printf(x"\n", ##__VA_ARGS__)
#define  PRINTMILLIS() PRINT("%.6f",(float)_mymillis()/1000)
#endif

#define TTRACE(x, ...) do { \
    PRINTMILLIS(); \
    myprintf(":" TAG "\t: " x, ##__VA_ARGS__); \
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

#define PCSERIALBAUD   115200
#define DEBUGSERIALBAUD  9600
#define GPSSERIALBAUD    9600

#endif // FUSEXUTIL_H
