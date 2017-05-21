#include <stdint.h>
#include <Arduino.h>

#define CRC16 0x8005

//Check routine taken from
//http://web.mit.edu/6.115/www/miscfiles/amulet/amulet-help/xmodem.htm
int calcrc16(const uint8_t *ptr, uint16_t count)
{
    int  crc;
    char i;
    crc = 0;

    do
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    } while (--count);

    return (crc);
}

uint16_t _gen_crc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = 0;
    int bits_read = 0, bit_flag;

    /* Sanity check: */
    if(data == NULL)
        return 0;

    while(size > 0)
    {
        bit_flag = out >> 15;

        /* Get next bit: */
        out <<= 1;
        out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

        /* Increment bit counter: */
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }

        /* Cycle check: */
        if(bit_flag)
            out ^= CRC16;

    }

    // item b) "push out" the last 16 bits
    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;
        out <<= 1;
        if(bit_flag)
            out ^= CRC16;
    }

    // item c) reverse the bits
    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }

    return crc;
}

void _myprintf(const __FlashStringHelper *fmt, ... )
{
    char buf[128]; // resulting string limited to 128 chars
    va_list args;

    va_start(args, fmt);
#ifdef __AVR__
    vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
    vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
    va_end(args);

    Serial.print(buf);
}

#define myprintf(a, ...) _myprintf(F(a), ##__VA_ARGS__)
#define TRACE(x, ...) myprintf(x, ##__VA_ARGS__)
#define TTRACE(x, ...) \
    myprintf("%6f:" x, (float)millis()/1000, ##__VA_ARGS__)

#ifdef CONFIG_DEBUG
#define DTRACE(x, ...)  TRACE(x, ##__VA_ARGS__)
#define DTTRACE(x, ...) TTRACE(x, ##__VA_ARGS__)
#else
#define DTRACE(x, ...) do {} while(0)
#define DTTRACE(x, ...) do {} while(0)
#endif

