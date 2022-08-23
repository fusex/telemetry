/****************************************************************
 * Example1_Basics.ino
 * ICM 20948 Arduino Library Demo
 * Use the default configuration to stream 9-axis IMU data
 * Owen Lyke @ SparkFun Electronics
 * Original Creation Date: April 17 2019
 *
 * Please see License.md for the license information.
 *
 * Distributed as-is; no warranty is given.
 ***************************************************************/
#include "ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU
#include <Arduino.h>
#include <avr/wdt.h>

#define SERIAL_PORT Serial

#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
#define AD0_VAL 1      // The value of the last bit of the I2C address.                \
                       // On the SparkFun 9DoF IMU breakout the default is 1, and when \
                       // the ADR jumper is closed the value becomes 0

ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object

void setup()
{
    SERIAL_PORT.begin(115200);
    while (!SERIAL_PORT){};
    delay(5000);

    WIRE_PORT.begin();
    WIRE_PORT.setClock(400000);

    myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

    bool initialized = false;
    while (!initialized)
    {
        myICM.begin(WIRE_PORT, AD0_VAL);

        SERIAL_PORT.print(F("Initialization of the sensor returned: "));
        SERIAL_PORT.println(myICM.statusString());
        if (myICM.status != ICM_20948_Stat_Ok)
        {
            SERIAL_PORT.println("Trying again...");
            delay(500);
        } else {
            initialized = true;
        }
    }
    while (1) {
        if (Serial.available() > 0) {
            // read the incoming byte:
            char incomingByte = Serial.read();
            if (incomingByte == 'r') {
                Serial.println("Reseting the board");
                ResetBoard();
            }
        }
        delay(1000);
    }
}

void ResetBoard ()
{
    //turn on the WatchDog and don't stroke it.
    wdt_enable(WDTO_15MS);
    while(1);
}

void loop()
{
    if (myICM.dataReady()) {
        myICM.getAGMT();         // The values are only updated when you call 'getAGMT'
        printScaledAGMT(&myICM); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
    }
    if (Serial.available() > 0) {
        // read the incoming byte:
        char incomingByte = Serial.read();
        if (incomingByte == 'r') {
            Serial.println("Reseting the board");
            ResetBoard();
        }
    }
    delay(1000);
}

void printScaledAGMT(ICM_20948_I2C *sensor)
{
#if 0
  SERIAL_PORT.print("Scaled. Acc (mg) [ ");
  printFormattedFloat(sensor->accX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->accY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->accZ(), 5, 2);
  SERIAL_PORT.print(" ], Gyr (DPS) [ ");
  printFormattedFloat(sensor->gyrX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->gyrY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->gyrZ(), 5, 2);
  SERIAL_PORT.print(" ], Tmp (C) [ ");
  printFormattedFloat(sensor->temp(), 5, 2);
#endif

  SERIAL_PORT.print("Mag (uT) [ ");
  printFormattedFloat(sensor->magX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->magY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->magZ(), 5, 2);
  SERIAL_PORT.print(" ]");
  SERIAL_PORT.println();
}

// Below here are some helper functions to print the data nicely!

void printPaddedInt16b(int16_t val)
{
  if (val > 0)
  {
    SERIAL_PORT.print(" ");
    if (val < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  else
  {
    SERIAL_PORT.print("-");
    if (abs(val) < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  SERIAL_PORT.print(abs(val));
}

void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
    float aval = abs(val);
    if (val < 0)
    {
        SERIAL_PORT.print("-");
    }
    else
    {
        SERIAL_PORT.print(" ");
    }
    for (uint8_t indi = 0; indi < leading; indi++)
    {
        uint32_t tenpow = 0;
        if (indi < (leading - 1))
        {
            tenpow = 1;
        }
        for (uint8_t c = 0; c < (leading - 1 - indi); c++)
        {
            tenpow *= 10;
        }
        if (aval < tenpow)
        {
            SERIAL_PORT.print("0");
        }
        else
        {
            break;
        }
    }
    if (val < 0)
    {
        SERIAL_PORT.print(-val, decimals);
    }
    else
    {
        SERIAL_PORT.print(val, decimals);
    }
}

