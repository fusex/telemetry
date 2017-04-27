#define TRACE(x) Serial.print(x)
#define TTRACE(x) do { \
	    Serial.print((float)millis()/1000,6); \
	    Serial.print(": "); \
	    Serial.print(x) ; \
    } while(0)

#ifdef DEBUG
#define DTRACE(x)  TRACE(x) 
#define DTTRACE(x) TTRACE(x)  
#else
#define DTRACE(x) do {} while(0)
#define DTTRACE(x) do {} while(0)
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

uint16_t _gen_crc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = 0;
    int bits_read = 0, bit_flag;

    TTRACE("ZSK T1\n");

TTRACE("ZSK T2 size:");
    TRACE(size);
    TRACE(" ... \n");

    /* Sanity check: */
    if(data == NULL)
        return 0;

    TTRACE("ZSK T2\n");
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
        } else {
    TTRACE("ZSK TXXXXX s:");
    TRACE(size);
    TRACE(" ... \n");

        }

        /* Cycle check: */
        if(bit_flag)
            out ^= CRC16;

    }
    TTRACE("ZSK T3\n");

    // item b) "push out" the last 16 bits
    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;
        out <<= 1;
        if(bit_flag)
            out ^= CRC16;
    }

    TTRACE("ZSK T4\n");
    // item c) reverse the bits
    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }
    TTRACE("ZSK T5\n");

    return crc;
}
