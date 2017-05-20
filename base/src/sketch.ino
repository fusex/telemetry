#include <fusexconfig.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <fusexutil.h>

RH_RF95 rf95;

void setup() 
{
    Serial.begin(9600);
    while (!Serial) ; // Wait for serial port to be available

    if (!rf95.init())
	TTRACE("init failed\n\r");
    else
	TTRACE("init Done\n\r");

#if 0 
  TTRACE("print reg: ");
  rf95.printRegisters();
  TRACE("\r");
#endif
    rf95.setTxPower(20,false);
    rf95.setFrequency(869.4);
#if 0
    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
    setSpreadingFactor(8);
#elif 1
    rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
    setSpreadingFactor(10);
#elif 1
    rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
    setSpreadingFactor(9);
#endif

#if 0 
  TTRACE("print reg: ");
  rf95.printRegisters();
  TRACE("\r");
#endif
}

boolean setSpreadingFactor(byte SF) { // abort and return FALSE if new spreading factor not in acceptable range; otherwise, set spreading factor and return TRUE
  uint8_t currentSF, newLowerNibble, newUpperNibble, current_register_1E_value, new_register_1E_value;
  if ((SF >= 6) && (SF <= 12)) {// only set if the spreading factor is in the proper range
    current_register_1E_value = rf95.spiRead(0x1E);
    Serial.print(F("Current value of RF95 register 0x1E = "));
    Serial.println(current_register_1E_value, HEX);
    currentSF = current_register_1E_value >> 4;  //upper nibble of register 0x1E
    Serial.print(F("Current spreading factor = "));
    Serial.println(currentSF, HEX);
    newLowerNibble = (current_register_1E_value & 0x0F); //same as old lower nibble
    newUpperNibble = (SF << 4); // upper nibble equals new spreading factor
    new_register_1E_value = (newUpperNibble + newLowerNibble);
    rf95.spiWrite(0x1E, new_register_1E_value);
    Serial.print(F("New spreading factor = "));
    Serial.println(SF);
    Serial.print(F("New value of register 0x1E = "));
    Serial.println(new_register_1E_value, HEX);
    return true;
  }
  else {
    return false;
  }
}

void send_ready(unsigned int packetnbr)
{
    char buf[RH_RF95_MAX_MESSAGE_LEN];

    sprintf(buf, "OK for %d", packetnbr);

    DTTRACE("Send Ready command to requester\n\r");
    rf95.send((uint8_t*)buf, sizeof(buf));
    rf95.waitPacketSent();
}

void printdata(char* data, unsigned int size)
{
    for(int i=0;i<9;i++) {
	Serial.print("byte ");
	Serial.print(i);
	Serial.print(" = ");
	Serial.println(data[i]);
    }
}

void receivepacket(int crc, unsigned int packetnbr)
{
    uint8_t errors = 0;
    int count = packetnbr;
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

    TTRACE("Start reception of packet\n\r");
    long now = millis();
    //while (1) {
    while (count--) {
	if (rf95.waitAvailableTimeout(10000)) {
	    uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
	    if (rf95.recv(buf, &len))
	    	DTTRACE("Received packet \n\r");
		if(crc) {
		    int ccrc = calcrc16(buf, len);
		    if (crc != ccrc) {
			TTRACE("ERROR: packet crc mismatch! ccrc: 0x");
			Serial.print(crc, HEX);
			TRACE(" ccrc: 0x");
			Serial.print(ccrc, HEX);
			TRACE("\n\r");
			printdata((char*)buf,len);

			if(!strncmp((char*)buf, "TEST BW for", 11)) {
			    goto error;
		        }
			errors++;
		    }
		}
	} else {
error:
	    TTRACE("ERROR: reception not completed ! ");
	    TRACE(packetnbr-count-1);
	    TRACE("/");
	    TRACE(packetnbr);
	    TRACE(" \n\r");
	    break;
        }
    }
    long delta = millis() - now;

    unsigned long datasize = (packetnbr-count-1) * RH_RF95_MAX_MESSAGE_LEN;

    TTRACE("Reception of ");
    TRACE(packetnbr-count-1);
    TRACE("/");
    TRACE(packetnbr);
    TRACE(" packet[s] completed in ");
    TRACE(delta);
    TRACE("ms ["); 
    TRACE((float)datasize*8/delta);
    TRACE( "kbps] with ");
    TRACE(errors);
    TRACE(" errors[s]\n\r");
}


uint16_t once = 0;

void loop()
{
    char buf[RH_RF95_MAX_MESSAGE_LEN];

    if(!once++)
    {
	TTRACE("#########################\n\r");
    	TTRACE("Waiting for Connection\n\r");
    }

    if (rf95.available())
    {
	uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
	once = 0;
	if (rf95.recv((uint8_t*)buf, &len)) {
	    unsigned int count = 0;
	    int crc = 0;
	    DTTRACE("Received command(len:");
            DTRACE(len);
	    DTRACE("): "); 
	    if (len)
	    	DTRACE(buf);
	    else
		DTRACE("- null - ");
	    DTRACE(" from a peer\n\r");

	    if(!strncmp(buf, "TEST BW for", 11)) {
		sscanf(buf, "TEST BW for %i CRC:%x", &count, &crc);

		TTRACE("Get Test Bandwidth command for ");
		TRACE(count);
		TRACE(" packet[s] with crc: 0x");
		Serial.print(crc, HEX);
		TRACE("\n\r");
		TTRACE("SNR: ");
		Serial.println(rf95.lastSNR(), DEC);
		TRACE("\r");
		TTRACE("RSSI: ");
		Serial.println(rf95.lastRssi(), DEC);
		TRACE("\r");
		TTRACE("Freq ERROR: ");
		Serial.println(rf95.frequencyError(), DEC);
		TRACE("\r");
		TTRACE("max length: ");
		Serial.println(rf95.maxMessageLength(), DEC);
		TRACE("\r");
		if(count) {
		    send_ready(count);
		    if(!crc)
		    	TTRACE("CRC check disabled!\n\r");
		    receivepacket(crc, count);
		}
	    }
	}
    }
    delay(400);
}
