#include <fusexconfig.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <fusexutil.h>

RH_RF95 rf95;

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

void setup() 
{
    randomSeed(analogRead(0));
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

//SF 6 does not works properly
//SF 7 8 works properly
//SF 9 work with errors for 125khz and properly for 500khz!
//SF 10 does not works properly for 125khz, and properly for 500khz
//SF 11 does not works properly for 125khz, and works with errors for 500khz
//SF 12 does not works properly for either 125khz and 500khz


#if 0 
  TTRACE("print reg: ");
  rf95.printRegisters();
  TRACE("\r");
#endif

}

void gendata(uint8_t* data, unsigned int size)
{
    while (size--){
	*data++ = random(32,126);
    }
}

void printdata(char* data, unsigned int size)
{
    for(int i=0;i<RH_RF95_MAX_MESSAGE_LEN;i++) {
	Serial.print("byte ");
	Serial.print(i);
	Serial.print(" = ");
	Serial.println(data[i]);
    }
}

void send_testcmd(int crc, unsigned int packetnbr)
{
    bool replied = false;
    char expected_rply[RH_RF95_MAX_MESSAGE_LEN];
    char buf[RH_RF95_MAX_MESSAGE_LEN];

    TTRACE("Preparing peer for transfer of ");
    TRACE(packetnbr);
    TRACE(" packet[s] of crc: 0x");
    Serial.print(crc, HEX);
    TRACE("\n\r");

    do {
	sprintf(buf, "TEST BW for %d CRC:0x%x", packetnbr, crc);
	rf95.send((uint8_t*)buf, sizeof(buf));
	rf95.waitPacketSent();

	DTTRACE("send command: \"");
	DTRACE(buf);
	DTRACE("\"\n\r");

	if (rf95.waitAvailableTimeout(3000)) { 
	    uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
	    memset(buf, 0, sizeof(buf));

	    if (rf95.recv((uint8_t*)buf, &len)) {
		sprintf(expected_rply, "OK for %d", packetnbr);
		if(!strncmp(buf, expected_rply, len))
		    replied = true;
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
	    }
	} else {
	    TTRACE("still waiting for peer response !\n\r");
	}
    } while(!replied);

    TTRACE("Peer is OK, start transfer!\n\r");
}

void loop()
{
    int crc = 0;
    unsigned int packetnbr = CONFIG_PACKETNUMBER;
    unsigned int count; 
    uint8_t data[RH_RF95_MAX_MESSAGE_LEN];

    gendata(data, sizeof(data));
    //printdata((char*)data, sizeof(data));
    crc = calcrc16(data,sizeof(data));

#ifdef CONFIG_CHECKCRC
    send_testcmd(crc, packetnbr);
#else
    send_testcmd(0, packetnbr);
#endif

    count = packetnbr;
    while (count--){
    //while (1){
#if 0
	rf95.send(data, sizeof(data));
#else
	rf95.send(data, 36);
#endif
	rf95.waitPacketSent();
    }

    TTRACE("Transfer of ");
    TRACE(packetnbr-count-1);
    TRACE("/");
    TRACE(packetnbr);
    TRACE(" to peer finished!\n\r");

    TTRACE("#########################\n\r");
    delay(2000);
}
