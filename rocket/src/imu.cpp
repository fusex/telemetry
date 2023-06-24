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
        BOOTTRACE("init ICM20948 Failed! fatal !!!\r\n");
        failed = true;
    }

    if (initICM20600() == false) {
        BOOTTRACE("init ICM20600 Failed! fatal !!!\r\n");
        failed = true;
    }

    if (failed == false) {
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
        ICM_20948_AGMT_t agmt = myImu.getAGMT();

        imuraw_t a[] = {agmt.acc.axes.x, agmt.acc.axes.y, agmt.acc.axes.z};
        imuraw_t m[] = {agmt.mag.axes.x, agmt.mag.axes.y, agmt.mag.axes.z};
        imuraw_t g[] = {agmt.gyr.axes.x, agmt.gyr.axes.y, agmt.gyr.axes.z};

        fxtm_setimu(a, m, g);
    } else {
        fxtm_seterror(FXTM_ERROR_IMU);
    }

    imuraw_t a2[] = {
        myImuAux.getRawAccelerationX(),
        myImuAux.getRawAccelerationY(),
        myImuAux.getRawAccelerationZ()
    };

    fxtm_setimu2(a2);
}
