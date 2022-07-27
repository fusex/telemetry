#include <Wire.h>
#include <math.h>

#define SIZE 1024
bool DEBUG = false;

// AUTO-ZERO
float P0;
uint16_t Pc0;
uint8_t t0 = 5;

// CALIBRATION
float T_min = -50.0, T_max = 150.0;
float p_min = -2.00012207031, p_max = 1.99981689453;
uint16_t Tcounts_min = 0, Tcounts_max = 2047;
uint16_t pcounts_min = 1638, pcounts_max = 14745;

// RING BUFFER
int pcounts[SIZE], Tcounts[SIZE], i;
long paccu = 0, Taccu = 0;

// I2C COMM
int address = 0x28;
uint8_t rD1, rD2, rD3, rD4;
uint8_t status;
uint16_t Pc, Tc;

// VALUES
float p, phpa, pmes, T, Tmes, v, vmax, vmin;
float f = 50;

// VALUES FUNCTIIONS
float countstoval(uint16_t Vc, uint16_t minc, uint16_t maxc, float minv, float maxv)
{
    return ((Vc - minc) * (maxv - minv)) / (maxc - minc) + minv;
}

float rho(float T)
{
    return 1.292 * (273.15 / (T + 273.15));
}

float velocity(float dp, float T)
{
    if (T <= -100) {
        return copysign(1.0, dp) * sqrt(2 * sqrt(pow(dp, 2)) / 1.21);
    } else {
        return copysign(1.0, dp) * sqrt(2 * sqrt(pow(dp, 2)) / rho(T));
    }
}

float calibrateSensor (int t0)
{
    uint16_t currentTime = millis();
    uint16_t actualTime = millis();
    uint16_t j = 0;
    while (currentTime - actualTime <= t0 * 1000) {
        status = readSensor();
        if ( status == 0 ) {
            Pc0 += Pc;
            j += 1;
        }
        actualTime = millis();
    }
    Pc0 /= j;
    return countstoval(Pc0, pcounts_min, pcounts_max, p_min, p_max);
}

int readSensor()
{
    Wire.beginTransmission(address);  // set sensor target
    Wire.requestFrom(address, 4);    // request four byte
    if (Wire.available() <= 4) {
        // receive 1 ACK + 1 Byte
        rD1 = Wire.read();
        // send 1 ACK + receive 1 Byte
        rD2 = Wire.read();
        // send 1 ACK + receive 1 Byte
        rD3 = Wire.read();
        // send 1 ACK + receive 1 Byte
        rD4 = Wire.read();
        // send 1 NACK + 1 STOP
        Wire.endTransmission(false);
    }
    if (DEBUG == true) {
        Serial.println(rD1, BIN);
        Serial.println(rD2, BIN);
        Serial.println(rD3, BIN);
        Serial.println(rD4, BIN);
    }
    status = (rD1 >> 6 & 0x3);
    //bit pressure is the last 6 bits of byte 0 (high bits) & all of byte 1 (lowest 8 bits)
    Pc = ( (uint16_t)rD1 << 8 & 0x3F00 ) | ( (uint16_t)rD2 & 0xFF );
    //bit temperature is all of byte 2 (lowest 8 bits) and the first three bits of byte 3
    Tc = ( ((uint16_t)rD3 << 3) & 0x7F8 ) | ( ( (uint16_t)(rD4) >> 5) & 0x7 );
    if (DEBUG == true) {
        Serial.println(Pc);
        Serial.println(Tc);
    }
    return status;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Init. Arduino OK.\n");
    Wire.begin();
    Serial.println("Init. Arduino OK.\n");
    P0 = calibrateSensor(10);
    Serial.println("Init. Arduino OK.\n");
    //delay(10);
    for (i = 0; i < SIZE;) {
        status = readSensor();
        if ( status == 0 ) {
            pcounts[i] = Pc;
            Tcounts[i] = Tc;
            paccu += pcounts[i];
            Taccu += Tcounts[i];
            i++;
        }
    }
    Serial.println("Init. Arduino OK.\n");
}

void loop()
{
    status = readSensor();
    if ( status == 0 ) {
        if (i == SIZE) {
            i = 0;
        }
        paccu -= pcounts[i];
        Taccu -= Tcounts[i];
        pcounts[i] = Pc;
        Tcounts[i] = Tc;
        paccu += pcounts[i];
        Taccu += Tcounts[i];
        pmes = paccu / SIZE;
        Tmes = Taccu / SIZE;
        p = countstoval(pmes, pcounts_min, pcounts_max, p_min, p_max);
        T = countstoval(Tmes, Tcounts_min, Tcounts_max, T_min, T_max);
        phpa = p * 1000.0;
        v = -3.6 * velocity((p - P0) * 100000, T);
        i += 1;
    }
    Serial.print("Différence de pression:");Serial.println((p-P0) * 100000, 1);
    Serial.print("Temperature:");Serial.println(T);
    Serial.print("Vitesse:");Serial.println(v);
    //delay((1000 / f) * 2);
    delay(500);
}

