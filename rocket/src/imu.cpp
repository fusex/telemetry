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
#define TAG "IMU"

#include "init.h"
#include <fusexconfig.h>
#include <fusexutil.h>

#include "Wire.h"
#include "MPU9250.h"

#include "BGC_Pinout.h"
#include "BGC_I2c.h"
#include "trame.h"

#define MPU9250_ADDRESS BGC_I2C_MAIN_IMU_ADDR

MPU9250 myIMU(MPU9250_ADDRESS);

#if ADO
#define MPU_AM_I_RET 0x71
#else
#define MPU_AM_I_RET 0x73
#endif
#define AK_AM_I_RET 0x48

#if 0
#define IMU_DEBUG 1 
#endif

static int initMPU ()
{
    byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);

    if (c != MPU_AM_I_RET){
	TTRACE("MPU9250 I AM 0x%x. I should be 0x%x\r\n", c, MPU_AM_I_RET);
	return false;
    }

#if IMU_CALIBRATION
    TTRACE("calibrating MPU ....\r\n");
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
#endif

    myIMU.initMPU9250();

    myIMU.getAres();
    myIMU.getGres();

    return true;
}

static int initAK ()
{
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    if (d != AK_AM_I_RET){
        TTRACE("AK8963 I AM 0x%x I should be 0x%x\r\n",d, AK_AM_I_RET);
        return false;
    }

    myIMU.getMres();
    myIMU.initAK8963(myIMU.factoryMagCalibration);

#if IMU_CALIBRATION
    TTRACE("calibrating AK ....\r\n");
    myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);
#endif

    return true;
}

void setupIMU ()
{
    Wire.begin();
    if (!initMPU())
    {
        TTRACE("init Failed! fatal !!!\r\n");
        Init_SetSemiFatal();
        return;
    }
    if (!initAK())
    {
        TTRACE("init Failed! fatal !!!\r\n");
        Init_SetFailed();
        return;
    }

    TTRACE("init Done.\r\n");
}

static int intrIMU ()
{
    return (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01);
}

void loopIMU ()
{
    if(!intrIMU()) return;

    unsigned long time = micros();
    unsigned long d1;

    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.readGyroData(myIMU.gyroCount);    // Read the x/y/z adc values
    myIMU.readMagData(myIMU.magCount);      // Read the x/y/z adc values
    myIMU.tempCount = myIMU.readTempData(); // Read the adc values

    d1 = micros() - time;

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

    float a[] = {myIMU.ax, myIMU.ay, myIMU.az};
    float m[] = {myIMU.mx/1000, myIMU.my/1000, myIMU.mz/1000}; 
    float g[] = {myIMU.gx, myIMU.gy, myIMU.gz};

    fxtm_setimu(a, m, g);
    DTRACE("ZSK packet acquired in:%ld and prepared in %ld us\r\n", d1, micros()-time);

    myIMU.updateTime();
    myIMU.count = millis();
}
