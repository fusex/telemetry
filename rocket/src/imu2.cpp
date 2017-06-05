/*
 * =====================================================================================
 *
 *       Filename:  imu.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:23:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#include <fusexconfig.h>
#include <fusexutil.h>

#include <I2Cdev.h>
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"
#include "Wire.h"

HMC5883L mag;
MPU6050  accelgyro;
BMP085   barometer;

float temperature;
float pressure;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

void setupImu()
{
#ifndef CONFIG_IMU
    return;
#endif

  Wire.begin();
  accelgyro.initialize();
  mag.initialize();
  barometer.initialize();

  accelgyro.setI2CBypassEnabled(true);

  if(!mag.testConnection() || 
     !barometer.testConnection() ||
     !accelgyro.testConnection()) {
	TTRACE("IMU: initialization failed! fatal !!!\r\n");
	while(1);
  }
}

static const float ACCEL_SENS = 16384.0; // Accel Sensitivity with default +/- 2g scale
static const float GYRO_SENS  = 131.0;   // Gyro Sensitivity with default +/- 250 deg/s scale
void loopImu()
{
#ifndef CONFIG_IMU
    return;
#endif

    uint32_t delta;
    uint32_t now = micros();
    uint32_t lastMicros;

    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    mag.getHeading(&mx, &my, &mz);

    barometer.setControl(BMP085_MODE_TEMPERATURE);
    lastMicros = micros();
    while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());
    temperature = barometer.getTemperatureC();

    barometer.setControl(BMP085_MODE_PRESSURE_3);
    while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());
    pressure = barometer.getPressure();

    delta = micros() - now;
 #define TOSTR0(x) dtostrf(x,2,2,tmp0)
 #define TOSTR1(x) dtostrf(x,2,2,tmp1)
 #define TOSTR2(x) dtostrf(x,2,2,tmp2)
    char tmp0[16];
    char tmp1[16];
    char tmp2[16];
    TTRACE("b++++++++++++++++++++++++++++++++++++++\r\n");
    TTRACE("i2clat:%lu Temp:%s Pression:%s altitude:%s\r\n", delta, TOSTR0(temperature), TOSTR1(pressure/100), TOSTR2(barometer.getAltitude(pressure)));
    TTRACE("ax:%s ay:%s az:%s\r\n", TOSTR0((float)ax/ACCEL_SENS), TOSTR1((float)ay/ACCEL_SENS), TOSTR2((float)az/ACCEL_SENS));
    TTRACE("gx:%s gy:%s gz:%s\r\n", TOSTR0(gx/GYRO_SENS), TOSTR1(gy/GYRO_SENS), TOSTR2(gz/GYRO_SENS));
    TTRACE("mx:%s my:%s mz:%s\r\n", TOSTR0(mx), TOSTR1(my),TOSTR2(mz));

    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    TTRACE("heading: %s degree\r\n", TOSTR0(heading * 180/M_PI));
    TTRACE("e++++++++++++++++++++++++++++++++++++++\r\n");

    delay(1000);
}
