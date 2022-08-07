#define TAG "PITOT"

#include <Wire.h>
#include <BGC_I2c.h>
#include <fusexconfig.h>
#include <fusexutil.h>
#include <trame.h>

#include "init.h"

// I2C COMM
static uint16_t Pc, Tc;
static uint8_t  PitotStatus = 0;
static bool     separation = false;

static fxtm_error_t readSensor ()
{
    uint8_t rD1, rD2, rD3, rD4;

    Wire.beginTransmission(BGC_I2C_DIFFPRESSURE_ADDR);  // set sensor target
    Wire.requestFrom(BGC_I2C_DIFFPRESSURE_ADDR, 4);     // request 4 bytes

    uint8_t available = Wire.available();
    if (available != 4) {
        return FXTM_FAILURE; // return as error as sensor is not responding correctly.
    }

    // receive 1 ACK + 1 Byte
    rD1 = Wire.read();
    // send 1 ACK + receive 1 Byte
    rD2 = Wire.read();
    // send 1 ACK + receive 1 Byte
    rD3 = Wire.read();
    // send 1 ACK + receive 1 Byte
    rD4 = Wire.read();
    // send 1 NACK + 1 STOP

    Wire.endTransmission();

    PitotStatus = (rD1 >> 6 & 0x3);
    //bit pressure is the last 6 bits of byte 0 (high bits) & all of byte 1 (lowest 8 bits)
    Pc = ( (uint16_t)rD1 << 8 & 0x3F00 ) | ( (uint16_t)rD2 & 0xFF );
    //bit temperature is all of byte 2 (lowest 8 bits) and the first three bits of byte 3
    Tc = ( ((uint16_t)rD3 << 3) & 0x7F8 ) | ( ( (uint16_t)(rD4) >> 5) & 0x7 );

    return FXTM_SUCCESS;
}

void setupPitot ()
{
    module_add(TAG);
    TTRACE("init Done.\r\n");
    module_setup(TAG, FXTM_SUCCESS);
}

void loopPitot ()
{
    fxtm_error_t ret = readSensor();

    if (ret == FXTM_FAILURE) {
        if (separation == false) {
            separation = true;
            fxtm_setflightstatus(FXTM_FLIGHTSTATUS_SEPARATION);
            DYNTTRACE(" SEPARATION detected\r\n");
        }
    } else {
        if (PitotStatus == 0) {
            if (separation == true) {
                separation = false;
                DYNTTRACE(" REINSERTION detected\r\n");
            }
            fxtm_settemperature(Tc);
            fxtm_setdiffpressure(Pc);
        } else {
            DYNTTRACE(" Error PitotStatus:0x%x\r\n", PitotStatus);
        }
    }
}
