#include <NMEAGPS.h>

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( NMEAGPS_PARSE_RMC )
  #error You must uncomment NMEAGPS_PARSE_RMC in NMEAGPS_cfg.h!
#endif

#if !defined( GPS_FIX_LOCATION )
  #error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

#if !defined( GPS_FIX_SATELLITES )
  #error You must uncomment GPS_FIX_SATELLITES in GPSfix_cfg.h!
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

//------------------------------------------------------------

static NMEAGPS gps; // This parses the GPS characters

//----------------------------------------------------------------

static void doSomeWork( const gps_fix & fix )
{
    Serial2.print( "GF:" );
    Serial2.print( sizeof(gps.fix()) );
    Serial2.print( ',' );
    Serial2.print( "NGO:" );
    Serial2.println( sizeof(gps) );
    Serial2.print( ',' );

    if (fix.valid.satellites)
        Serial2.print( fix.satellites );
    else
        Serial2.print( '?' );

    Serial2.print( ',' );

    if (fix.valid.location) {
        Serial2.print( fix.latitude(), 6 ); // floating-point display
        Serial2.print( fix.latitudeL() ); // integer display
        Serial2.print( ',' );
        Serial2.print( fix.longitude(), 6 ); // floating-point display
        Serial2.print( fix.longitudeL() );  // integer display
        Serial2.print( ',' );
        Serial2.print( fix.altitude() ); 

    } else {
        // No valid location data yet!
        Serial2.print( '?' );
        Serial2.print( ',' );
        Serial2.print( '?' );
        Serial2.print( ',' );
        Serial2.print( '?' );
    }
    Serial2.println();

} // doSomeWork

//------------------------------------

void setup()
{
    Serial2.begin(115200);
    while (!Serial2);
    delay(10000);

    Serial2.print( F("NMEAloc.INO: started\n") );
    Serial2.print( F("fix object size = ") );
    Serial2.println( sizeof(gps.fix()) );
    Serial2.print( F("nmeagps object size = ") );
    Serial2.println( sizeof(gps) );
    Serial2.println( F("Looking for GPS device on Serial1") );

    Serial1.begin(9600);
}

void loop()
{
    while (gps.available(Serial1))
        doSomeWork( gps.read() );
}
