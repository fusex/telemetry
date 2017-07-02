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

#if 0
#define TAG "IMU"

#include <fusexconfig.h>
#include <fusexutil.h>

#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>
#include <Wire.h>

#include "trame.h"

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
  Wire.begin();
  accelgyro.initialize();
  mag.initialize();
  barometer.initialize();

  accelgyro.setI2CBypassEnabled(true);

  if(!mag.testConnection() || 
     !barometer.testConnection() ||
     !accelgyro.testConnection()) {
	TTRACE("initialization failed! fatal !!!\r\n");
	//while(1);
  }
  TTRACE("initialization Done.\r\n");
}

#define ACCEL_SENS (16384) // Accel Sensitivity with default +/- 2g scale
#define GYRO_SENS  (131)   // Gyro Sensitivity with default +/- 250 deg/s scale

void loopImu()
{
    uint32_t now;
    uint32_t lastMicros;

    now = micros();
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    mag.getHeading(&mx, &my, &mz);

    barometer.setControl(BMP085_MODE_TEMPERATURE);
    lastMicros = micros();
    while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());
    temperature = barometer.getTemperatureC();

    barometer.setControl(BMP085_MODE_PRESSURE_3);
    lastMicros = micros();
    while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());
    pressure = barometer.getPressure();

    int16_t a[] = {(int16_t)ax, (int16_t)ay, (int16_t)az};
    int16_t m[] = {(int16_t)mx, (int16_t)my, (int16_t)mz}; 
    int16_t g[] = {(int16_t)gx, (int16_t)gy, (int16_t)gz};

    fxtm_setimu(a, m, g);
    fxtm_settemperature((int8_t)temperature);
    fxtm_setpressure(GET_RAWPRESSURE((int)pressure));
 
#if 0
    TTRACE("b++++++++++++++++++++++++++++++++++++++\r\n");
    WTTRACE("IMU data acquired in %ld\r\n", micros() - now); 
 #define TOSTR0(x) dtostrf(x,2,2,tmp0)
 #define TOSTR1(x) dtostrf(x,2,2,tmp1)
 #define TOSTR2(x) dtostrf(x,2,2,tmp2)
    char tmp0[16];
    char tmp1[16];
    char tmp2[16];
    TTRACE("Temp:%s Pression:%s altitude:%s\r\n", TOSTR0(temperature), TOSTR1(pressure), TOSTR2(barometer.getAltitude(pressure)));
    TTRACE("ax:%s ay:%s az:%s\r\n", TOSTR0((float)ax/ACCEL_SENS), TOSTR1((float)ay/ACCEL_SENS), TOSTR2((float)az/ACCEL_SENS));
    TTRACE("mx:%s my:%s mz:%s\r\n", TOSTR0(mx), TOSTR1(my),TOSTR2(mz));
    TTRACE("gx:%s gy:%s gz:%s\r\n", TOSTR0(gx/GYRO_SENS), TOSTR1(gy/GYRO_SENS), TOSTR2(gz/GYRO_SENS));

    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    TTRACE("heading: %s degree\r\n", TOSTR0(heading * 180/M_PI));

    TTRACE("++++++++++++++++++++++++++++++++++++++\r\n");
#endif

    WTTRACE("e++++++++++++++++++++++++++++++++++++++\r\n");
}

#else
#if 0
void setupImu()
{
}

void loopImu()
{
}
#endif
#endif
