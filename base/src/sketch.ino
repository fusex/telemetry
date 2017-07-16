#define BOARD_UNO 1

#include <fusexconfig.h>
#include <fusexutil.h>
#include <trame.h>

#include <SPI.h>
#include <RH_RF95.h>
#include <fusexutil.h>

RH_RF95 rf95;

void setup() 
{
    Serial.begin(9600);
    //Serial.begin(115200);
    while (!Serial) ; // Wait for serial port to be available

    if (!rf95.init()){
	TTRACE("Radio init failed\n\r");
	while(1);
    }
    else
	TTRACE("Radio init Done with packet size:%d\n\r",sizeof(fxtm_data_t));

    rf95.setTxPower(20,false);
    rf95.setFrequency(869.4);
    rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);

#if 0
    setSpreadingFactor(10);
#elif 1
    setSpreadingFactor(9);
#endif
}

static boolean setSpreadingFactor(byte SF)
{
    // abort and return FALSE if new spreading factor not in acceptable range;
    // otherwise, set spreading factor and return TRUE
    uint8_t currentSF, newLowerNibble, newUpperNibble, cur_reg_val, new_reg_val;

    if ((SF >= 6) && (SF <= 12)) {
	// only set if the spreading factor is in the proper range
	cur_reg_val = rf95.spiRead(0x1E);

	DTTRACE("Current value of RF95 register 0x1E = 0x%x\r\n",cur_reg_val);

	//upper nibble of register 0x1E
	currentSF = cur_reg_val >> 4;
	DTTRACE("Current spreading factor = 0x%x\r\n", cur_reg_val);

	//same as old lower nibble
	newLowerNibble = (cur_reg_val & 0x0F);

	// upper nibble equals new spreading factor
	newUpperNibble = (SF << 4);
	new_reg_val = (newUpperNibble + newLowerNibble);
	rf95.spiWrite(0x1E, new_reg_val);

	DTTRACE("New spreading factor = %d, New value of register 0x1E = %x\r\n",
		SF, new_reg_val);

	return true;
    }

    return false;
}

bool     once = true;
//uint8_t  buf[RH_RF95_MAX_MESSAGE_LEN];
uint16_t lastid = 0;
uint32_t lastts = 0;

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
	    WTTRACE("ERROR: reception Error at %d/%d! Timeout !\n\r", packetnbr-count-1, packetnbr);
	    break;
        }
    }

    return count;
}

void fxtmcheck()
{
    fxtm_data_t* tm = fxtm_getdata();

    if (tm->id != (lastid +1) && tm->id != 0) {
	TTRACE("discontinuation at id: %u at ts: %lu, lastid:%u lastts:%lu\r\n",
	       tm->id, tm->timestamp, lastid, lastts);
	TTRACE("SNR: %d RSSI: %d Freq ERROR: %d\r\n",
	       rf95.lastSNR(),
	       rf95.lastRssi(),
	       rf95.frequencyError()
	);
    }
    lastid = tm->id;
    lastts = tm->timestamp;
}

void dumpstat()
{
    fxtm_dump(); 
}

uint32_t timer = millis();
void loop()
{
    if (once) {
	TTRACE("#########################\n\r");
	TTRACE("Waiting for Connection\n\r");
	once = false;
    }
    if (timer > millis())  timer = millis();
    if (millis() - timer > 2000) {
	timer = millis(); // reset the timer
	dumpstat();
    }

    uint32_t now = micros();
    if (rf95.available()) {
        uint32_t d1 = micros() - now;
	if(receivepacket(CONFIG_PACKETNUMBER)) {
#if 0
	    fxtm_dump();
#endif
	    fxtmcheck();
	    //Serial.write(buf, sizeof(fxtm_data_t));
	}
	
	WTTRACE("d1:%lu d2:%lu\r\n", d1, micros() - now);
    }
}
