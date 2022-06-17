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

#include "BGC_Pinout.h"
#include "BGC_I2c.h"
#include "trame.h"

#include <ICM20948_WE.h>
#include "ICM20600.h"

ICM20948_WE myImu = ICM20948_WE(BGC_I2C_MAIN_IMU_ADDR);
ICM20600    myImuAux = ICM20600(false);

static int initICM20948 ()
{
    if (!myImu.init())
	return false;

    if (!myImu.initMagnetometer())
	return false;

#if IMU_CALIBRATION
    myImu.autoOffsets();
#endif

    myImu.setGyrRange(ICM20948_GYRO_RANGE_250);
    myImu.setAccRange(ICM20948_ACC_RANGE_16G);
    myImu.setAccDLPF(ICM20948_DLPF_6);    
    myImu.setGyrDLPF(ICM20948_DLPF_6);  
    myImu.setAccSampleRateDivider(10);
    myImu.setGyrSampleRateDivider(10);
    myImu.setMagOpMode(AK09916_CONT_MODE_10HZ);

    return true;
}

static int initICM20600 ()
{
    myImuAux.initialize();
    myImuAux.reset();

    return true;
}

void setupIMU ()
{
    Wire.begin();
    if (!initICM20948())
    {
        TTRACE("init ICM20948 Failed! fatal !!!\r\n");
        Init_SetSemiFatal();
        return;
    }

    if (!initICM20600())
    {
        TTRACE("init ICM20600 Failed! fatal !!!\r\n");
        Init_SetSemiFatal();
        return;
    }

    TTRACE("init Done.\r\n");
}

void loopIMU ()
{
    unsigned long time = micros();
    unsigned long d1;

    myImu.readSensor();
#if 0
    xyzFloat acc = myImu.getAccRawValues();
    xyzFloat gyr = myImu.getGyrValues();
#else
    xyzFloat acc = myImu.getCorrectedAccRawValues();
    xyzFloat gyr = myImu.getCorrectedGyrRawValues();
#endif
    xyzFloat mag = myImu.getMagValues();

    float a[] = {acc.x, acc.y, acc.z};
    float m[] = {mag.x, mag.y, mag.z}; 
    //float m[] = {mag.x/1000, mag.y/1000, mag.z/1000}; 
    float g[] = {gyr.x, gyr.y, gyr.z};
    float a2[] = {
	    myImuAux.getAccelerationX(),
	    myImuAux.getAccelerationY(),
	    myImuAux.getAccelerationZ()
    };

    fxtm_setimu(a, m, g);
    fxtm_setimu2(a2);
    DTRACE("ZSK packet acquired in:%ld and prepared in %ld us\r\n", d1, micros()-time);
}
