#include <BGC_Lora.h>
#include <fusexutil.h>
#include <trame.h>

static BGC_Lora lora(10, true); //Lora shield on DUE.

static bool once = true;

static unsigned int receivepacket(unsigned int packetnbr) {
    unsigned int count = packetnbr;

    WTTRACE("Start reception of packet\n\r");

    while (count--) {
        if (lora.waitAvailableTimeout(10000)) {
            uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
            if (lora.recv((uint8_t*)fxtm_getdata(), &len))
                DTTRACE("Received packet \n\r");
        } else {
            TTRACE("ERROR: reception Error at %d/%d! Timeout !\n\r",
                   packetnbr - count - 1, packetnbr);
            break;
        }
    }

    return count;
}

void setup() {
    DEBUGdevice.begin(DEBUGSERIALBAUD);
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("Fusex base Receiver Start\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");

    if (!lora.init()) {
        TTRACE("Radio init failed\n\r");
        return;
    } else
        TTRACE("Radio init Done with packet size:%d\n\r", fxtm_getdatasize());
}

void loop() {
    if (once) {
        TTRACE("#########################\n\r");
        TTRACE("Waiting for Connection\n\r");
        TTRACE("pc transfer packet size: %d\n\r", sizeof(fxtm_data_t));
        once = false;
    }

    if (lora.available()) {
        if (receivepacket(CONFIG_PACKETNUMBER)) {
            if (fxtm_check(NULL)) {
                TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n", lora.lastSNR(),
                       lora.lastRssi(), lora.frequencyError());
            }
            fxtm_dump(false);
        }
    }
}
