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
#include <fusexconfig.h>
#include <fusexutil.h>

#include "MPU9250.h"

#define SerialDebug true  // Set to true to get Serial output for debugging

MPU9250 myIMU;
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins

void setupImu()
{
#ifndef CONFIG_IMU
    return;
#endif

  Wire.begin();
  // TWBR = 12;  // 400 kbit/sec I2C speed

  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x71, HEX);

  if (c == 0x71) // WHO_AM_I should always be 0x71
  {
    Serial.println("MPU9250 is online...");

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    myIMU.initMPU9250();

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    Serial.print("AK8963 ");
    Serial.print("I AM ");
    Serial.print(d, HEX);
    Serial.print(" I should be ");
    Serial.println(0x48, HEX);

    return true;
}


    myIMU.getMres();

#if IMU_CALIBRATION
    TTRACE("calibrating AK ....\r\n");
    myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);
#endif

    return true;
}

void setupIMU()
{
    Wire.begin();
    if(!initMPU()){
	TTRACE("initialization failed! fatal !!!\r\n");
	fatal();
    }
    if(!initAK()){
	TTRACE("initialization failed! fatal !!!\r\n");
	fatal();
    }
    if(!initBMP()){
	TTRACE("initialization failed! fatal !!!\r\n");
	fatal();
    }

  } // if (c == 0x71)
  else
  {
    Serial.print("Could not connect to MPU9250: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }
}

void loopImu()
{
#ifndef CONFIG_IMU
    return;
#endif
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
      unsigned long time = micros();
      unsigned long d1;

    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.tempCount = myIMU.readTempData();  // Read the adc values

    d1 = micros() - time;

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes; // - myIMU.accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes; // - myIMU.accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes; // - myIMU.accelBias[2];

    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;

    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];

    myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;

    TRACE("ZSK packet acquired in:%ld and prepared in %ld us\r\n", d1, micros()-time);

    // Print acceleration values in milligs!
    Serial.print("X-acceleration: "); Serial.print(1000 * myIMU.ax);
    Serial.print(" mg ");
    Serial.print("Y-acceleration: "); Serial.print(1000 * myIMU.ay);
    Serial.print(" mg ");
    Serial.print("Z-acceleration: "); Serial.print(1000 * myIMU.az);
    Serial.println(" mg ");

    // Print gyro values in degree/sec
    Serial.print("X-gyro rate: "); Serial.print(myIMU.gx, 3);
    Serial.print(" degrees/sec ");
    Serial.print("Y-gyro rate: "); Serial.print(myIMU.gy, 3);
    Serial.print(" degrees/sec ");
    Serial.print("Z-gyro rate: "); Serial.print(myIMU.gz, 3);
    Serial.println(" degrees/sec");

    // Print mag values in degree/sec
    Serial.print("X-mag field: "); Serial.print(myIMU.mx);
    Serial.print(" mG ");
    Serial.print("Y-mag field: "); Serial.print(myIMU.my);
    Serial.print(" mG ");
    Serial.print("Z-mag field: "); Serial.print(myIMU.mz);
    Serial.println(" mG");


    // Print temperature in degrees Centigrade
    Serial.print("Temperature is ");  Serial.print(myIMU.temperature, 1);
    Serial.println(" degrees C");
    myIMU.updateTime();
    myIMU.count = millis();

  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

}
#endif 
