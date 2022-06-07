#include <fusexconfig.h>
#include <fusexutil.h>
#include <radioparam.h>
#include <trame.h>

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
static bool once = true;
#define DEBUG

static unsigned int receivepacket(unsigned int packetnbr)
{
    unsigned int count = packetnbr;

    WTTRACE("Start reception of packet\n\r");

    while (count--) {
	if (rf95.waitAvailableTimeout(10000)) {
	    uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
	    if (rf95.recv((uint8_t*)fxtm_getdata(), &len))
	    	DTTRACE("Received packet \n\r");
	} else {
	    TTRACE("ERROR: reception Error at %d/%d! Timeout !\n\r", packetnbr-count-1, packetnbr);
	    break;
        }
    }

    return count;
}

void setup() 
{
    DEBUGdevice.begin(DEBUGSERIALBAUD);
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("Fusex base Receiver Start\n\r");
    TTRACE("--------------------------------------\n\r");
    TTRACE("--------------------------------------\n\r");

    PCdevice.begin(PCSERIALBAUD);

    while (!PCdevice) ; // Wait for serial port to be available

    if (!rf95.init()){
	TTRACE("Radio init failed\n\r");
	return;
    } else
	TTRACE("Radio init Done with packet size:%d\n\r",fxtm_getdatasize());

    //fx_setTxPower();
    fx_setFrequency();
    fx_setModemConfig();
    fx_setSpreadingFactor();
}

void loop()
{
    if (once) {
	TTRACE("#########################\n\r");
	TTRACE("Waiting for Connection\n\r");
	TTRACE("pc transfer packet size: %d\n\r",sizeof(fxtm_data_t));
	once = false;
    }

#ifdef PROFILING 
    uint32_t now = micros();
#endif
    if (rf95.available()) {
	if(receivepacket(CONFIG_PACKETNUMBER)) {
#ifdef PROFILING
            uint32_t d1 = micros() - now;
#endif
#ifdef DEBUG 
	    if(fxtm_check(NULL)) {
		TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n",
			rf95.lastSNR(),
			rf95.lastRssi(),
			rf95.frequencyError()
		      );
	    }
	    fxtm_dump();
#endif
	    int bytesSent = PCdevice.write((char*)fxtm_getdata(), fxtm_getdatasize());
#ifdef DEBUG
	    TTRACE("PLOP sent:%d, expected:%d\r\n",bytesSent, fxtm_getdatasize()); 
#endif
#ifdef PROFILING
	    WTTRACE("d1:%lu d2:%lu\r\n", d1, micros() - now);
#endif
	}
    }
}
