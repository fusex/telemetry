#include <string.h>

#include "fusexutil.h"

#if 0
#if _IS_BASE
static SoftwareSerial SWSerial(2, 3);
#endif
#endif

#define CRC16 0x8005

static bool GdynTrace = false;

void dynTrace_enable(bool enable)
{
    GdynTrace = enable;
}

bool dynTrace_isEnable()
{
    return GdynTrace;
}

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
        if(*n == '%' && (
            *(n+1) == 'f' ||
            ( *(n+2) == 'f' && isdigit(*(n+1)))
            )) {
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

    DEBUGPRINTS(buf);
}

long _myrandom(long min, long max)
{
#if defined(_IS_HOST)
   return min + random()%(max-min);
#else
   return random(min,max);
#endif
}

unsigned long _mymillis()
{
#if defined(_IS_HOST)
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (tp.tv_sec * 1000000 + tp.tv_usec)/1000;
#else
   return millis();
#endif
}

char consolebuf[CONSOLE_BUF_SIZE];

void _mycprintf(const _FMT_FLASH_TYPE *fmt, ... )
{
    va_list args;

    va_start(args, fmt);
    VSNPRINTF(consolebuf, sizeof(consolebuf), (const char *)fmt, args);
    va_end(args);

    SHELLPRINTS(consolebuf);
}

void _myprintf(const _FMT_FLASH_TYPE *fmt, ... )
{
    va_list args;

    va_start(args, fmt);
    VSNPRINTF(consolebuf, sizeof(consolebuf), (const char *)fmt, args);
    va_end(args);

    DEBUGPRINTS(consolebuf);
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
    if (size == 0)
        return;

    while (size--) {
        *data = _myrandom(32, 126);
         data++;
    }
}

typedef struct {
    char*         module_name;
    fxtm_error_t  setup_error;
    fxtm_error_t  run_error;
} fxtm_module_t;

#define MAX_MODULES 16
static fxtm_module_t fxtm_modules[MAX_MODULES];
static uint8_t       fxtm_modules_count = 0;

void module_add(const char* tag)
{
    if (fxtm_modules_count == MAX_MODULES) {
        TTRACE("ERROR maximum number of modules reached.\r\n");
        fatal();
    }

    fxtm_modules[fxtm_modules_count++].module_name = (char*) tag;
}

void module_setup(const char* tag, fxtm_error_t error)
{
    for (uint8_t i =0; i<fxtm_modules_count; i++) {
        if (fxtm_modules[i].module_name == tag) {
            fxtm_modules[i].setup_error = error;
            break;
        }
    }

    if (error == FXTM_SUCCESS) {
        BOOTTRACE("%s init Done.\r\n", tag);
    } else {
        BOOTTRACE("%s init Failed.\r\n", tag);
    }
}

void modules_printall(bool isConsole)
{
    for (uint8_t i =0; i<fxtm_modules_count; i++) {
        MYTRACE("%-10s \t: %s\r\n",
                fxtm_modules[i].module_name,
                fxtm_modules[i].setup_error==FXTM_SUCCESS?"Success":"Failure");
    }
}

#ifdef DEBUG
void printdata(char* data, unsigned int size)
{
    for(int i=0;i<size;i++) {
        DEBUGdevice.print("byte ");
        DEBUGdevice.print(i);
        DEBUGdevice.print(" = ");
        DEBUGdevice.println(data[i]);
    }
}
#endif
