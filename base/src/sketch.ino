#include <fusexconfig.h>
#include <fusexutil.h>
#include <radioparam.h>
#include <trame.h>

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

void setup() 
{
    DEBUGdevice.begin(DEBUGSERIALBAUD);
    PCdevice.begin(PCSERIALBAUD);

    while (!PCdevice) ; // Wait for serial port to be available

    if (!rf95.init()){
	TTRACE("Radio init failed\n\r");
#if 0
	while(1);
#else
	return;
#endif
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
	fxtm_dump(NULL); 
    }
}


#undef DEBUG
#if 0
#define DEBUG 1
#else
#define DEBUG 0
#endif


typedef struct {
    float imu[9];
    float gps[2];

    int32_t  pressure;
    uint32_t ts;
    uint16_t id;
    uint8_t  sndlvl;
    int8_t   temp;
} __attribute__((packed)) vivien_trame_t;


vivien_trame_t vt;

void vt_prepare()
{
   fxtm_getimu(vt.imu);
   fxtm_getgps(vt.gps);
   fxtm_getpressure(&vt.pressure);
   fxtm_getts(&vt.ts);
   fxtm_getid(&vt.id);
   fxtm_getsoundlvl(&vt.sndlvl);
   fxtm_gettemperature(&vt.temp);
}

void vt_send()
{
    vt_prepare();
    PCdevice.write('$');
    PCdevice.write((uint8_t*)&vt, sizeof(vivien_trame_t));
    PCdevice.println();
}

void raw_send()
{
    PCdevice.write((uint8_t*)fxtm_getdata(), fxtm_getdatasize());
}

void loop()
{
#if 1
    if (once) {
	TTRACE("#########################\n\r");
	TTRACE("Waiting for Connection\n\r");
	TTRACE("pc transfer packet size: %d\n\r",sizeof(vivien_trame_t));
	once = false;
    }
#endif

#if DEBUG
    dumpstat();
#endif
   
    uint32_t now = micros();
#if 0
    if (rf95.available()) {
        uint32_t d1 = micros() - now;
	if(receivepacket(CONFIG_PACKETNUMBER)) {
#else
/* ZSK TOREMOVE */
	if(1) {
	    if(1) {
		fxtm_data_t* tm = fxtm_getdata();
		tm->timestamp = 48232; 
		tm->id = 104; 
		fxtm_setgps(43.242376,-0.039298);
		fxtm_setpressure(96646);
		fxtm_settemperature(40);
		fxtm_setsoundlvl(512);
/* ZSK END */
#endif
    
#if DEBUG
	    if(fxtm_check()) {
		TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n",
			rf95.lastSNR(),
			rf95.lastRssi(),
			rf95.frequencyError()
		      );
	    }
#else

#if 0

	   //raw_send();
	   vt_send();
	   fxtm_dump(NULL);
           //delay(100);
#else
/* ZSK TOREMOVE */
	    fxtm_dump(NULL);
	    while(1) {
		int bytesSent = PCdevice.write((uint8_t*)fxtm_getdata(), fxtm_getdatasize());
		TTRACE("PLOP sent:%d, expected:%d\r\n",bytesSent, fxtm_getdatasize()); 
		delay(30000);
	    } ;
#endif

/* ZSK END */

#endif
	}
	
	WTTRACE("d1:%lu d2:%lu\r\n", d1, micros() - now);
    }
}
