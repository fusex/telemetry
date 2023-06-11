#define TAG "BASE"

#include <BGC_Lora.h>
#include <fusexutil.h>
#include <trame.h>

#if 0
# define TRAME_DUMP 1
#endif

#if 1
# define TRAME_CHECK 1
#endif

//FIXME: fxtmp_dump() does not completly display all the packet.
#if TRAME_DUMP
#define fxbase_dump() do { \
    static char buf[256]; \
    fxtm_dumpdata(fxtm_getdata(), buf, 256); \
    TTRACE("%s \n\r", buf); \
} while(0)
#else
#define fxbase_dump()
#endif

#ifdef TRAME_CHECK
# define fxbase_check() do { \
    uint32_t lastts; \
    uint16_t lastid; \
    uint16_t ret = fxtm_check(NULL, &lastid, &lastts); \
    if (ret != 0) { \
        uint32_t ts; \
        uint16_t id; \
        fxtm_getrxts(NULL, &ts); \
        fxtm_getid(NULL, &id); \
        TTRACE("Discontinuation at id: %u at ts: %u, lastid:%u lastts:%u\r\n", \
               id, ts, lastid, lastts); \
        TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n", lora.lastSNR(), \
                lora.lastRssi(), lora.frequencyError()); \
    } \
} while(0);
#else
# define fxbase_check()
#endif
 
static BGC_Lora lora(10, true); //Lora shield on Arduino DUE or Arduino Mega.

static uint32_t packetcounter = 0;

static boolean receivepacket (void)
{
    WTTRACE("Start reception of packet\n\r");

    if (lora.waitAvailableTimeout(10000)) {
        uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
        if (lora.recv((uint8_t*)fxtm_getdata(), &len)) {
            DTTRACE("Received packet \n\r");
            if (len < fxtm_getdatasize() ) {
                TTRACE("Warning: packet incomplete at %d!\n\r", packetcounter);
            }
            fxtm_rxfooter_t* rxf = fxtm_getrxfooter();
            rxf->timestamp = _mymillis();
            rxf->snr = lora.lastSNR();
            rxf->rssi = lora.lastRssi();
            rxf->frequencyError = lora.frequencyError();
            PCdevice.write((const char*)fxtm_getdata(), fxtm_getrxdatasize());
        }
    } else {
        TTRACE("ERROR: reception Error at %d! Timeout !\n\r", packetcounter);
        return false;
    }

    TRACE("%d\r", packetcounter++);

    return true;
}

void setup()
{
    DEBUGdevice.begin(DEBUGSERIALBAUD);
    PCdevice.begin(PCSERIALBAUD);
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("Fusex base Receiver Start\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");

    while (true) {
        if (lora.init()) {
            TTRACE("Radio init Done\n\r");
            break;
        } else {
            TTRACE("Radio init failed\n\r");
            delay(2000);
        }
    }

    TTRACE("#########################\n\r");
    TTRACE("Radio transfer packet size:%d\n\r", fxtm_getdatasize());
    TTRACE("   PC transfer packet size:%d\n\r", fxtm_getrxdatasize());
    TTRACE("Waiting for packets .... \n\r");
}

void loop()
{
    if (lora.available()) {
        if (receivepacket()) {
            fxbase_check();
            fxbase_dump();
        }
    }
}
