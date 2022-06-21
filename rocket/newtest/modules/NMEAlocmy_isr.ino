#include <NMEAGPS.h>
#include <NeoHWSerial.h>

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( GPS_FIX_LOCATION )
  #error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

#if !defined( NMEAGPS_INTERRUPT_PROCESSING )
  #error You must define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

//------------------------------------------------------------

static NMEAGPS gps; // This parses the GPS characters

//----------------------------------------------------------------

static void GPSisr( uint8_t c )
{
  gps.handle( c );

} // GPSisr

static void printAll( const gps_fix & fix )
{
#if 0
    Serial2.print( fix.dateTime.seconds );
#endif
    Serial2.print( ',' );
    Serial2.print( "GF:" );
    Serial2.print( sizeof(gps.fix()) );
    Serial2.print( ',' );
    Serial2.print( "NGO:" );
    Serial2.print( sizeof(gps) );
    Serial2.print( ',' );

#if 0
    if (fix.valid.satellites)
        Serial2.print( fix.satellites );
    else
        Serial2.print( '?' );
#endif

    Serial2.print( ',' );

    if (fix.valid.location) {
        Serial2.print( fix.latitude(), 6 ); // floating-point display
        Serial2.print( fix.latitudeL() ); // integer display
        Serial2.print( ',' );
        Serial2.print( fix.longitude(), 6 ); // floating-point display
        Serial2.print( fix.longitudeL() );  // integer display
    } else {
        // No valid location data yet!
        Serial2.print( '?' );
        Serial2.print( ',' );
        Serial2.print( '?' );
    }
    Serial2.print( ',' );

#if 0
    if (fix.valid.altitude) {
        Serial2.print( fix.altitude() ); 
    } else {
        Serial2.print( '?' );
    }
#endif

    Serial2.println();

} // printAll

//------------------------------------

#define NeoSerial(x) Neo##x

void setup()
{
    Serial2.begin(115200);
    while (!Serial2);

    Serial2.print( F("NMEAloc.INO: started\n") );
    Serial2.print( F("fix object size = ") );
    Serial2.println( sizeof(gps.fix()) );
    Serial2.print( F("nmeagps object size = ") );
    Serial2.println( sizeof(gps) );
    Serial2.println( F("Looking for GPS device on Serial1") );

#if 1
    NeoSerial(Serial1).attachInterrupt( GPSisr );
    NeoSerial(Serial1).begin(9600);
#else
  //--configure UART1 of MEGA: Bd = 9600; RXRDY interrupt----
  UCSR1B = 0b10011000;  //
  UCSR1C = 0b00000110;  //
  //-Bd: 9600-------------
  UBRR1L = 0x67;
  UBRR1H = 0x00;
  //--global int enable--
  bitSet(SREG, 7);
#endif
}

#if 0
ISR(USART1_RX_vect)
{
  gps.handle( UDR1 );
}
#endif

void loop()
{
    if (gps.available()) {
        Serial2.println() ;
        printAll( gps.read() );
    }
    delay(100);
    Serial2.print(".") ;
#if 1
    if (gps.overrun()) {
        gps.overrun( false );
        Serial2.print("O") ;
    }
#endif
}
