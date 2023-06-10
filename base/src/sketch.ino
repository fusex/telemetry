#define TAG "BASE"

#include <BGC_Lora.h>
#include <fusexutil.h>
#include <trame.h>

static BGC_Lora lora(10, true); //Lora shield on DUE.

//FIXME: fxtmp_dump() does not completly display all the packet.
#if 0
#define fxtmp_dump() do { \
    static char buf[256]; \
    fxtm_dumpdata(fxtm_getdata(), buf, 256); \
    TTRACE("%s \n\r", buf); \
} while(0)
#else
#define fxtmp_dump()
#endif

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
            fxtm_rxheader_t* rxData = fxtm_getrxheader();
            rxData->timestamp = _mymillis();
            rxData->snr = lora.lastSNR();
            rxData->rssi = lora.lastRssi();
            rxData->frequencyError = lora.frequencyError();
            PCdevice.write((const char*)fxtm_getdata(), fxtm_getrxdatasize());
            fxtmp_dump();
            packetcounter++;
        }
    } else {
        TTRACE("ERROR: reception Error at %d! Timeout !\n\r", packetcounter);
        return false;
    }

    TRACE("%d\r", packetcounter);

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
            if (fxtm_check(NULL)) {
                TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n", lora.lastSNR(),
                       lora.lastRssi(), lora.frequencyError());
            }
        }
    }
}
