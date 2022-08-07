#define TAG "IMU"

#include <fusexconfig.h>
#include <fusexutil.h>
#include <Wire.h>
#include <BGC_Pinout.h>
#include <BGC_I2c.h>
#include <trame.h>
#include <ICM20948_WE.h>
#include "ICM20600.h"

#include "init.h"

#define isAD0(x) (x&1)
ICM20948_WE myImu = ICM20948_WE(BGC_I2C_MAIN_IMU_ADDR);
ICM20600    myImuAux = ICM20600(isAD0(BGC_I2C_AUX_IMU_ADDR));

#if 1
#define IMU_CALIBRATION
#endif

static int initICM20948 ()
{
    if (myImu.init() == false)
        return false;

#ifdef IMU_CALIBRATION
    myImu.autoOffsets();
#endif

    myImu.setGyrRange(ICM20948_GYRO_RANGE_250);
    myImu.setGyrDLPF(ICM20948_DLPF_6);  
    myImu.setGyrSampleRateDivider(10);

    myImu.setAccRange(ICM20948_ACC_RANGE_16G);
    myImu.setAccDLPF(ICM20948_DLPF_6);    
    myImu.setAccSampleRateDivider(10);

    return true;
}

static int initAK ()
{
    if (!myImu.initMagnetometer())
        return false;

    myImu.setMagOpMode(AK09916_CONT_MODE_20HZ);

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
    module_add(TAG);

    bool failed = false;
    Wire.begin();

    if (initICM20948() == false)
    {
        TTRACE("init ICM20948 Failed! fatal !!!\r\n");
        failed = true;
    }

#if 1
    if (initAK() == false)
    {
        TTRACE("init ICM20948-AK Failed! fatal !!!\r\n");
        failed = true;
    }
#endif

    if (initICM20600() == false)
    {
        TTRACE("init ICM20600 Failed! fatal !!!\r\n");
        failed = true;
    }

    if (failed == false) {
        TTRACE("init Done.\r\n");
        module_setup(TAG, FXTM_SUCCESS);
    } else {
        module_setup(TAG, FXTM_FAILURE);
        Init_SetSemiFatal();
    }
}

void loopIMU ()
{
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
}
