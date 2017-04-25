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
}

void send_ready(unsigned int packetnbr)
{
    char buf[RH_RF95_MAX_MESSAGE_LEN];

    sprintf(buf, "OK for %d", packetnbr);

    DTTRACE("Send Ready command to requester\n\r");
    rf95.send((uint8_t*)buf, sizeof(buf));
    rf95.waitPacketSent();
}

void receivepacket(int crc, unsigned int packetnbr)
{
    uint8_t errors = 0;
    int count = packetnbr;
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

    TTRACE("Start reception of packet\n\r");
    long now = millis();
    while (count--) {
	if (rf95.waitAvailableTimeout(3000)) {
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

			errors++;
		    }
		}
	} else {
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
