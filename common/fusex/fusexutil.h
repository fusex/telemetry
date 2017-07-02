#ifndef FUSEXUTIL_H
#define FUSEXUTIL_H

#include <stdint.h>
#include <Arduino.h>

#include <fusexconfig.h>

int calcrc16(const uint8_t *ptr, uint16_t count);
uint16_t _gen_crc16(const uint8_t *data, uint16_t size);
void _myprintf(const __FlashStringHelper *fmt, ... );
void gendata(uint8_t* data, unsigned int size);
void fatal(void);

#define myprintf(a, ...) _myprintf(F(a), ##__VA_ARGS__)

#ifndef TAG
#define TAG "unknown-module"
#endif

#define TRACE(x, ...) myprintf(x, ##__VA_ARGS__)
#define TTRACE(x, ...) do { \
    DEBUGdevice.print((float)millis()/1000,6); \
    myprintf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#ifdef CONFIG_DEBUG
#define DTRACE(x, ...)  TRACE(x, ##__VA_ARGS__)
#define DTTRACE(x, ...) TTRACE(x, ##__VA_ARGS__)
void printdata(char* data, unsigned int size);
#else
#define DTRACE(x, ...) do {} while(0)
#define DTTRACE(x, ...) do {} while(0)
#endif

#ifdef CONFIG_TRACES
#define WTRACE(x, ...)  TRACE(x, ##__VA_ARGS__)
#define WTTRACE(x, ...) TTRACE(x, ##__VA_ARGS__)
void printdata(char* data, unsigned int size);
#else
#define WTRACE(x, ...) do {} while(0)
#define WTTRACE(x, ...) do {} while(0)
#endif

#endif // FUSEXUTIL_H
