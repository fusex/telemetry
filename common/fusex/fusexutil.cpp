/*
 * =====================================================================================
 *
 *       Filename:  fusexutil.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/05/2017 22:06:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#include <string.h>

#include "fusexutil.h"

#if _IS_BASE
SoftwareSerial SWSerial(2, 3);
#endif

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

void fatal()
{
#if CONFIG_FATAL
    while(1);
#endif
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

char* mystrchr(char* fmt)
{
    char* n = fmt;
    while(1){
	if(*n == '%' &&
	    ( *(n+1) == 'f' ||
	      ( *(n+2) == 'f' && isdigit(*(n+1)))
            )
          )
        {
	   return n;
	}
	if(*n==0)
	    return NULL;
        n++;
    }
}

void _myprintf2(const char *fmt, ... )
{
    char *p = (char*)fmt;
    char bufString[128]; // resulting string limited to 128 chars
    char* buf = bufString;
    va_list args;

    va_start(args, fmt);
    do {
	unsigned int factor = 100;
        char *q = mystrchr(p);
        if (q) {
            *q++ = '\0';
            char f = *q++;
            if(isdigit(f)) {
		factor = pow(10, f-'0');
		q++;
	    }
        }
	buf += vsnprintf(buf, strlen(buf), (const char *)fmt, args); // for the rest of the world
        if (q) {
	    double f = va_arg(args, double);
            buf += sprintf(buf, "%d.%d", (int)f,(int)(f*factor)%factor);
        }
        p = q;
    } while (p);

    va_end(args);

    PRINTS(buf);
}

long _myrandom(long min, long max)
{
#if defined(_IS_PC)
   return min + random()%(max-min);
#else
   return random(min,max);
#endif
}

unsigned long _mymillis()
{
#if defined(_IS_PC)
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (tp.tv_sec * 1000000 + tp.tv_usec)/1000;
#else
   return millis();
#endif
}

void _myprintf(const _FMT_FLASH_TYPE *fmt, ... )
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

    PRINTS(buf);
}

#if 0
char *ftoa(char *a, double f, int precision = 100)
{
    long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};

    char *ret = a;
    long heiltal = (long)f;
    itoa(heiltal, a, 10);
    while (*a != '\0') a++;
    *a++ = '.';
    long desimal = abs((long)((f - heiltal) * p[precision]));
    itoa(desimal, a, 10);
    return ret;
}
#endif

void gendata(uint8_t* data, unsigned int size)
{
    while (size--){
        *data++ = _myrandom(32, 126);
    }
}

#ifdef DEBUG
void printdata(char* data, unsigned int size)
{
    for(int i=0;i<RH_RF95_MAX_MESSAGE_LEN;i++) {
	DEBUGdevice.print("byte ");
	DEBUGdevice.print(i);
	DEBUGdevice.print(" = ");
	DEBUGdevice.println(data[i]);
    }
}
#endif