#include <fusexutil.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <SdFat.h>
#include <SPIMemory.h>

#define LOOP         200000UL
#define LED_DATAIO   31
#define LED_SDCARDL  32
#define LED_GPS      33
#define LED_RADIO    34
#define LED_IMU      35
#define LED_FERROR   36
#define LED_MIN      LED_DATAIO
#define LED_MAX      LED_FERROR

#define BUZZER          A12

#define SD_CS_PIN       11
#define SPIFLASH_CS_PIN 10

#define LORA_CS_PIN     25
#define LORA_RST_PIN    27
#define LORA_DIO0        2

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int tempo[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// Jingle Bells

int jinglebells_melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};

int jinglebells_tempo[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};

// We wish you a merry Christmas
int wish_melody[] = {
  NOTE_B3,
  NOTE_F4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4,
  NOTE_D4, NOTE_D4, NOTE_D4,
  NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4,
  NOTE_E4, NOTE_E4, NOTE_E4,
  NOTE_A4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_D4, NOTE_B3, NOTE_B3,
  NOTE_D4, NOTE_G4, NOTE_E4,
  NOTE_F4
};

int wish_tempo[] = {
  4,
  4, 8, 8, 8, 8,
  4, 4, 4,
  4, 8, 8, 8, 8,
  4, 4, 4,
  4, 8, 8, 8, 8,
  4, 4, 8, 8,
  4, 4, 4,
  2
};

// Santa Claus is coming to town
int santa_melody[] = {
  NOTE_G4,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4,
  NOTE_D4, NOTE_F4, NOTE_B3,
  NOTE_C4
};

int santa_tempo[] = {
  8,
  8, 8, 4, 4, 4,
  8, 8, 4, 4, 4,
  8, 8, 4, 4, 4,
  8, 8, 4, 2,
  4, 4, 4, 4,
  4, 2, 4,
  1
};

SdFat SD;
File myFile;

void test_buzzer()
{
    for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(int)); thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / tempo[thisNote];

        tone(BUZZER, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;

        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(BUZZER);
    }
}

void test_buzzer1()
{
    for (int thisNote = 0; thisNote < (sizeof(santa_melody)/sizeof(int)); thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / santa_tempo[thisNote];

        tone(BUZZER, santa_melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;

        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(BUZZER);

    }
}

void test_buzzer2()
{
    for (int thisNote = 0; thisNote < (sizeof(wish_melody)/sizeof(int)); thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / wish_tempo[thisNote];

        tone(BUZZER, wish_melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;

        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(BUZZER);

    }
}

void test_buzzer3()
{
    for (int thisNote = 0; thisNote < (sizeof(jinglebells_melody)/sizeof(int)); thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / jinglebells_tempo[thisNote];

        tone(BUZZER, jinglebells_melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;

        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(BUZZER);

    }
}

void test_leds ()
{
    TRACE("Rocket LEDs test ");

    for (int testled=LED_MIN; testled <= LED_MAX; testled++) {
        pinMode(testled, OUTPUT);
    }

    for (int testled=LED_MIN; testled <= LED_MAX; testled++) {
        int ledloop = 0;
        boolean fledout = LOW;
        while (ledloop++<20) {
            digitalWrite(testled, fledout);
            fledout = !fledout;
            delay(100);
        }
        for (int led=LED_MIN; led <= LED_MAX; led++) {
            digitalWrite(led, HIGH);
            delay(200);
        }
        for (int led=LED_MIN; led <= LED_MAX; led++) {
            digitalWrite(led, LOW);
            delay(200);
        }
        for (int led=LED_MAX; led >= LED_MIN; led--) {
            digitalWrite(led, HIGH);
            delay(200);
        }
        for (int led=LED_MAX; led >= LED_MIN; led--) {
            digitalWrite(led, LOW);
            delay(200);
        }
        TRACE(".");
    }

    TRACE("OK\r\n");
}

void test_i2c()
{
    byte error, address;
    int nDevices;

    TRACE("I2C device Scanning ...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            TRACE(" 0x%x ..",address,HEX);
            nDevices++;
        }
        else if (error==4) {
            TRACE("Unknown Error 0x%x ..",address,HEX);
        }
    }
    if (nDevices == 0)
        TRACE("No I2C devices found\r\n");
    else
        TRACE(" OK\r\n");
}

SPIFlash flash(SPIFLASH_CS_PIN);
void printUniqueID(void)
{
  long long _uniqueID = flash.getUniqueID();

  if (_uniqueID) {
    DEBUGdevice.print("Unique ID: ");
    DEBUGdevice.print(uint32_t(_uniqueID / 1000000L));
    DEBUGdevice.print(uint32_t(_uniqueID % 1000000L));
    DEBUGdevice.print(", ");
    DEBUGdevice.print("0x");
    DEBUGdevice.print(uint32_t(_uniqueID >> 32), HEX);
    DEBUGdevice.print(uint32_t(_uniqueID), HEX);
  }
}

bool getID()
{
    DEBUGdevice.println();
    DEBUGdevice.print("SPIMemory Library version: ");
#ifdef LIBVER
    uint8_t _ver, _subver, _bugfix;
    flash.libver(&_ver, &_subver, &_bugfix);
    DEBUGdevice.print(_ver);
    DEBUGdevice.print(".");
    DEBUGdevice.print(_subver);
    DEBUGdevice.print(".");
    DEBUGdevice.println(_bugfix);
#else
    DEBUGdevice.println(F("< 2.5.0"));
#endif
    DEBUGdevice.println();
    uint32_t JEDEC = flash.getJEDECID();
    if (!JEDEC) {
        DEBUGdevice.println("No comms. Check wiring. Is chip supported? If unable to fix, raise an issue on Github");
        return false;
    }
    else {
        DEBUGdevice.print("JEDEC ID: 0x");
        DEBUGdevice.println(JEDEC, HEX);
        DEBUGdevice.print("Man ID: 0x");
        DEBUGdevice.println(uint8_t(JEDEC >> 16), HEX);
        DEBUGdevice.print("Memory ID: 0x");
        DEBUGdevice.println(uint8_t(JEDEC >> 8), HEX);
        DEBUGdevice.print("Capacity: ");
        DEBUGdevice.println(flash.getCapacity());
        DEBUGdevice.print("Max Pages: ");
        DEBUGdevice.println(flash.getMaxPage());
        printUniqueID();

    }
    return true;
}

RH_RF95 rf95(LORA_CS_PIN, LORA_DIO0);
void test_lora()
{
    TRACE("Rocket SPI (LORA) test");
    TRACE("..");
    int retry = 100;
    delay(5000);
    pinMode(LORA_RST_PIN, OUTPUT);
    
    while (retry-- > 0) {
	delay(1000);
	digitalWrite(LORA_RST_PIN, HIGH);
        if (!rf95.init()) {
            TRACE(" initialization failed! retry:%d \r\n", retry);
            delay(1000);
        } else {
            break;
        }
	delay(1000);
	digitalWrite(LORA_RST_PIN, LOW);
    }
    TRACE(" OK\r\n");
}

void test_flash()
{
    TRACE("Rocket SPI (Flash Memory) test ");
    if (flash.error()) {
        TRACE("%s\n\r",flash.error(VERBOSE));
    }
    TRACE("..");

    flash.begin();
    TRACE("..");
    getID();
    TRACE(" OK\r\n");
}

void test_sdcard()
{
    TRACE("Rocket SPI (SDCard) test ");
    if (!SD.begin(SD_CS_PIN)) {
        TRACE(" initialization failed!\r\n");
        return;
    }

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("test.txt", FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {
        myFile.println("testing 1, 2, 3.");
        // close the file:
        myFile.close();
        TRACE(".. writing ..");
    } else {
        // if the file didn't open, print an error:
        TRACE("error opening test.txt");
    }

    // re-open the file for reading:
    myFile = SD.open("test.txt");
    if (myFile) {
        TRACE(".. reading ..");

        // read from the file until there's nothing else in it:
        while (myFile.available()) {
            TRACE("%c",myFile.read());
        }
        // close the file:
        myFile.close();
    } else {
        // if the file didn't open, print an error:
        TRACE("error opening test.txt");
    }
}

void test_spi()
{
    test_sdcard();
    test_flash();
    test_lora();
}

void test_gps()
{
    TRACE("Rocket GPS test ");
    int c = 0;
    int f = 0;

    GPSdevice.begin(9600);

    while (!GPSdevice) {
        TTRACE("init failed ! Retrying !\n\r");
        delay(2000);
    }

    while (f == 0) {
        while (GPSdevice.available()) {
            TRACE("%c", (char)GPSdevice.read());
            //if (++c>1000) f = 1;
        }
        //delay(1000);
    }
    TRACE(" OK\r\n");
}

void setup()
{
    MCUSR = 0; // clear out any flags of prior resets.
    randomSeed(analogRead(0));

    pinMode(LED_FERROR, OUTPUT);
    digitalWrite(LED_FERROR, LOW);

    DEBUGdevice.begin(DEBUGSERIALBAUD);
    while (!DEBUGdevice) ; // Wait for DEBUG serial port to be available

    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);

    Wire.begin();
}

void loopALL()
{
    {
        TRACE("#######################\r\n");
        TRACE("Rocket Serial test OK\r\n");
    }

    //test_buzzer();
    //test_buzzer1();
    //test_buzzer3();

    test_buzzer2();
    test_leds();

    test_i2c();

    test_spi();
    test_gps();

    TRACE("All tests done successfully!\r\n");

    while (1) {
        delay(1000);
    }
}

void loop()
{
    {
        TRACE("#######################\r\n");
        TRACE("Rocket Serial test OK\r\n");
    }

    test_gps();

    TRACE("All tests done successfully!\r\n");

    while (1) {
        delay(1000);
    }
}

void loopLora()
{
    {
        TRACE("#######################\r\n");
        TRACE("Rocket Serial test OK\r\n");
    }

    test_lora();

    TRACE("All tests done successfully!\r\n");

    while (1) {
        delay(1000);
    }
}
