#include <fusexconfig.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <fusexutil.h>

RH_RF95 rf95;

void setup() 
{
    randomSeed(analogRead(0));
    Serial.begin(9600);
    while (!Serial) ; // Wait for serial port to be available

    if (!rf95.init())
	TTRACE("init failed\n\r");
    else
	TTRACE("init Done\n\r");
}

void gendata(uint8_t* data, unsigned int size)
{
    while (size--){
	*data++ = random(32,126);
    }
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
    crc = calcrc16(data,sizeof(data));

#ifdef CONFIG_CHECKCRC
    send_testcmd(crc, packetnbr);
#else
    send_testcmd(0, packetnbr);
#endif

    count = packetnbr;
    while (count--){
	rf95.send(data, sizeof(data));
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
