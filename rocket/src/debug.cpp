#include <Arduino.h>
#include <Wire.h>
#include <fusexutil.h>

void i2c_scanner() {
  int nDevices = 0;
  Wire.begin();

  TTRACE("Scanning...");

  for (byte address = 1; address < 127; ++address) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      TTRACE("I2C device found at address 0x%0x!\n", address);
      ++nDevices;
    } else if (error == 4) {
      TTRACE("Unknown error at address 0x%0x!\n", address);
    }
  }
  if (nDevices == 0) {
    TTRACE("No I2C devices found\n");
  } else {
    TTRACE("done\n");
  }
  delay(5000); // Wait 5 seconds for next scan
}