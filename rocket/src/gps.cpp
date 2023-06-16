#define TAG "GPS"

#include <NMEAGPS.h>
#include <NeoHWSerial.h>
#include <fusexutil.h>
#include <ublox/ubxGPS.h>
#include <BGC_Pinout.h>
#include <trame.h>

#include "init.h"

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( GPS_FIX_LOCATION )
  #error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

#if !defined( NMEAGPS_INTERRUPT_PROCESSING )
  #error You must define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

#define RETRYMAX 60 // Retry 60 secondes.
static NMEAGPS gps; // This parses the GPS characters

// The parser is expecting only GNGLL messages at 5Hz rate.
// Please use ubloxcfg.ino to configure the ublox gps receiver accordingly.

// Complicated macros to perform double indirection inorder to append
// "Neo" to the Serial name ex: BGC_SerialGPS == Serial1  => GPSdevice == NeoSerial1.
#define VARIABLE Neo
#define PASTER(x,y) x ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define NAME(fun) EVALUATOR(VARIABLE, fun)

#define GPSdevice NAME(BGC_SerialGPS)

static void GPSisr ( uint8_t c )
{
    gps.handle( c );
}

static int receiveGPS ()
{
    int ret = -1;

    if (gps.available()) {
        gps_fix fix = gps.read();
        if (fix.valid.location) {
            fxtm_setgps(fix.latitudeL(), fix.longitudeL());
            ret = 0; 
        }
    }

    return ret;
}

const unsigned char ubxRate5Hz[] PROGMEM =
  { 0x06,0x08,0x06,0x00,200,0x00,0x01,0x00,0x01,0x00 };
// Disable specific NMEA sentences
const unsigned char ubxDisableGGA[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableGSA[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableGSV[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableRMC[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableVTG[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableZDA[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x08,0x00,0x00,0x00,0x00,0x00,0x01 };
const uint32_t COMMAND_DELAY = 250;

void sendUBX (const unsigned char *progmemBytes, size_t len )
{
    GPSdevice.write( 0xB5 ); // SYNC1
    GPSdevice.write( 0x62 ); // SYNC2

    uint8_t a = 0, b = 0;
    while (len-- > 0) {
        uint8_t c = pgm_read_byte( progmemBytes++ );
        a += c;
        b += a;
        GPSdevice.write( c );
    }

    GPSdevice.write( a ); // CHECKSUM A
    GPSdevice.write( b ); // CHECKSUM B
    delay( COMMAND_DELAY );

} // sendUBX

static void configureUblox ()
{
    sendUBX( ubxDisableRMC, sizeof(ubxDisableRMC) );
    sendUBX( ubxDisableGSV, sizeof(ubxDisableGSV) );
    sendUBX( ubxDisableGSA, sizeof(ubxDisableGSA) );
    sendUBX( ubxDisableGGA, sizeof(ubxDisableGGA) );
    sendUBX( ubxDisableVTG, sizeof(ubxDisableVTG) );
    sendUBX( ubxDisableZDA, sizeof(ubxDisableZDA) );
    sendUBX( ubxRate5Hz, sizeof(ubxRate5Hz) );
}

void setupGps ()
{
    module_add(TAG);

    bool gpsFixed = false;

    GPSdevice.attachInterrupt(GPSisr);
    GPSdevice.begin(GPSSERIALBAUD);
    delay(500);
    configureUblox();

    int retry = 0;
    while (retry++ < RETRYMAX) {
        if (receiveGPS() == 0) {
            gpsFixed = true;
            break;
        }
        delay(1000);
    }

    //
    if (gpsFixed == false) {
        TTRACE("init Failed ! retry later.\r\n");
        Init_SetFailed();
    } else {
        TTRACE("init Done: retry:%d\r\n", retry);
        module_setup(TAG, FXTM_SUCCESS);
    }
}

void loopGps ()
{
    receiveGPS();
    if (gps.overrun()) {
        gps.overrun( false );
    }
}
