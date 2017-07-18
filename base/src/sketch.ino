#include <fusexconfig.h>
#include <fusexutil.h>
#include <radioparam.h>
#include <trame.h>

#include <SPI.h>
#include <RH_RF95.h>

#if 0
#define SERIALBAUD 9600
#else
#define SERIALBAUD 115200
#endif

RH_RF95 rf95;

void setup() 
{
    DEBUGdevice.begin(9600);
    PCdevice.begin(SERIALBAUD);
    while (!PCdevice) ; // Wait for serial port to be available

    if (!rf95.init()){
	TTRACE("Radio init failed\n\r");
	while(1);
    }
    else
	TTRACE("Radio init Done with packet size:%d\n\r",fxtm_getdatasize());

    //fx_setTxPower();
    fx_setFrequency();
    fx_setModemConfig();
    fx_setSpreadingFactor();
}

bool     once = true;
//uint8_t  buf[RH_RF95_MAX_MESSAGE_LEN];
int receivepacket(unsigned int packetnbr)
{
    uint8_t errors = 0;
    int     count = packetnbr;

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

uint32_t timer = millis();
void dumpstat()
{
    if (timer > millis())  timer = millis();
    if (millis() - timer > 2000) {
	timer = millis(); // reset the timer
	fxtm_dump(); 
    }
}



void loop()
{
    if (once) {
	TTRACE("#########################\n\r");
	TTRACE("Waiting for Connection\n\r");
	once = false;
    }

#if DEBUG
    dumpstat();
#endif
   
    uint32_t now = micros();
    if (rf95.available()) {
        uint32_t d1 = micros() - now;
	if(receivepacket(CONFIG_PACKETNUMBER)) {
#if DEBUG
	    if(fxtm_check()) {
		TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n",
			rf95.lastSNR(),
			rf95.lastRssi(),
			rf95.frequencyError()
		      );
	    }
#else
	    PCdevice.write((uint8_t*)fxtm_getdata(), fxtm_getdatasize());

	    fxtm_dump();
#endif
	}
	
	WTTRACE("d1:%lu d2:%lu\r\n", d1, micros() - now);
    }
}
