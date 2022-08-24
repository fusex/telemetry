#define TAG "IMU"

#include <fusexconfig.h>
#include <fusexutil.h>
#include <Wire.h>
#include <BGC_Pinout.h>
#include <BGC_I2c.h>
#include <trame.h>

#include <ICM_20948.h>
#include <ICM20600.h>

#include "init.h"

#define isAD0(x) (x&1)

ICM_20948_I2C myImu;
ICM20600      myImuAux = ICM20600(isAD0(BGC_I2C_AUX_IMU_ADDR));

static bool initICM20948 ()
{
    myImu.begin(Wire, isAD0(BGC_I2C_MAIN_IMU_ADDR));
    if (myImu.status != ICM_20948_Stat_Ok)
        return false;

    /* Reset the IMU in a known state */
    myImu.swReset();
    delay(250);
    myImu.sleep(false); 
    myImu.lowPower(false); 
    if (myImu.status != ICM_20948_Stat_Ok)
        return false;

    /* Set the scale to 16G for accel and 500 for Gyro */
    ICM_20948_fss_t myFSS =
    	{ .a = gpm16, .g = dps500 };
    myImu.setFullScale(ICM_20948_Internal_Acc|ICM_20948_Internal_Gyr, myFSS);
    if (myImu.status != ICM_20948_Stat_Ok)
        return false;

    /* Set the sampling rate to 10 Hz */
    myImu.setSampleMode(
	(ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr),
	ICM_20948_Sample_Mode_Cycled);
    ICM_20948_smplrt_t mySmplrt = {.a = 10, .g = 10};
    myImu.setSampleRate(ICM_20948_Internal_Acc|ICM_20948_Internal_Gyr, mySmplrt);
    if (myImu.status != ICM_20948_Stat_Ok)
        return false;

#if 1
    /* Enable a Filter */
    ICM_20948_dlpcfg_t myDLPcfg = 
    	{.a = acc_d11bw5_n17bw, .g = gyr_d11bw6_n17bw8};
    myImu.setDLPFcfg(ICM_20948_Internal_Acc|ICM_20948_Internal_Gyr, myDLPcfg);
    if (myImu.status != ICM_20948_Stat_Ok)
        return false;
#endif

    myImu.startupMagnetometer();
    if (myImu.status != ICM_20948_Stat_Ok)
        return false;

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
        CTRACE("init ICM20948 Failed! fatal !!!\r\n");
        failed = true;
    }

    if (initICM20600() == false) {
        CTRACE("init ICM20600 Failed! fatal !!!\r\n");
        failed = true;
    }

    if (failed == false) {
        CTRACE("init Done.\r\n");
        module_setup(TAG, FXTM_SUCCESS);
    } else {
        module_setup(TAG, FXTM_FAILURE);
        Init_SetSemiFatal();
    }
}

void loopIMU ()
{
    if (myImu.dataReady())
    {
        myImu.getAGMT();

        float a[] = {myImu.accX(), myImu.accY(), myImu.accZ()};
        float m[] = {myImu.magX(), myImu.magY(), myImu.magZ()};
        float g[] = {myImu.gyrX(), myImu.gyrY(), myImu.gyrZ()};

        fxtm_setimu(a, m, g);
    }

    float a2[] = {
        myImuAux.getAccelerationX(),
        myImuAux.getAccelerationY(),
        myImuAux.getAccelerationZ()
    };

    fxtm_setimu2(a2);
}
