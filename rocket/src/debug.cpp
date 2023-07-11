#include <Arduino.h>
#include <Wire.h>
#include <fusexutil.h>

void i2c_scanner (bool isConsole)
{
  int nDevices = 0;
  Wire.begin();

  MYTTRACE("Scanning...");

  for (byte address = 1; address < 127; ++address) {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();

      if (error == 0) {
          MYTRACE("I2C device found at address 0x%0x!\n\r", address);
          ++nDevices;
      } else if (error == 4) {
          MYTRACE("Unknown error at address 0x%0x!\n\r", address);
      }
  }
  if (nDevices == 0) {
      MYTRACE("No I2C devices found\n\r");
  } else {
      MYTRACE("done\n\r");
  }
  delay(5000); // Wait 5 seconds for next scan
}

uint8_t i2c_read (bool isConsole, uint8_t address, uint8_t reg)
{
    uint8_t value = 0xff;

    Wire.begin();
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address, 1);
    if(Wire.available()){
        value = Wire.read();
    } else {
        MYTRACE("RR");
    }

    return value;
}

void i2c_write (bool isConsole, uint8_t address, uint8_t reg, uint8_t value)
{
    Wire.begin();

    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

void hexdump (bool isConsole, uint32_t addr, const uint8_t* buf, size_t len)
{
    uint8_t *p;
    MYTRACE("%08lx:", addr);
    for (p = (uint8_t*) buf; len-- > 0; p++) {
        MYTRACE(" 0x%02x", *p);
        if (!(len%8)) {
            addr += 8;
            MYTRACE("\n\r %08lx:", addr);
        }
    }
    MYTRACE("\n\r");
}

void i2c_dump (bool isConsole, uint8_t address)
{
    MYTRACE("%02lx:", 0);
    for (byte reg = 0; reg < 255; ++reg) {
        uint8_t val = i2c_read (isConsole, address, reg);
        MYTRACE(" %02x", val);
        if (!(reg%8)) {
            MYTRACE("\n\r %02x:", reg);
        }
    }
    MYTRACE("\n\r");
}
