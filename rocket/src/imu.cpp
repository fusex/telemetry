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
#include "fxstatus.h"

#define isAD0(x) (x&1)

#if 1
#define IMU_DMP 1
#endif

static int8_t dmp_error = -1;

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

#ifdef IMU_DMP
    dmp_error = 0;
    do {
        if (myImu.initializeDMP() == ICM_20948_Stat_Ok) {
            BOOTTRACE("init initializeDMP Failed! skip !!!\r\n");
            dmp_error = 1;
            break;
        }
        if (myImu.enableDMPSensor(INV_ICM20948_SENSOR_ORIENTATION) == ICM_20948_Stat_Ok) {
            BOOTTRACE("init enableDMPSensor Failed! skip !!!\r\n");
            dmp_error = 2;
            break;
        }
        if (myImu.setDMPODRrate(DMP_ODR_Reg_Quat9, 0) == ICM_20948_Stat_Ok) {
            BOOTTRACE("init setDMPODRrate Failed! skip !!!\r\n");
            dmp_error = 3;
            break;
        }
        if (myImu.enableFIFO() == ICM_20948_Stat_Ok) {
            BOOTTRACE("init enableFIFO Failed! skip !!!\r\n");
            dmp_error = 4;
            break;
        }
        if (myImu.enableDMP() == ICM_20948_Stat_Ok) {
            BOOTTRACE("init enableDMP Failed! skip !!!\r\n");
            dmp_error = 5;
            break;
        }
        if (myImu.resetDMP() == ICM_20948_Stat_Ok) {
            BOOTTRACE("init resetDMP Failed! skip !!!\r\n");
            dmp_error = 6;
            break;
        }
        if (myImu.resetFIFO() == ICM_20948_Stat_Ok) {
            BOOTTRACE("init resetFIFO Failed! skip !!!\r\n");
            dmp_error = 7;
            break;
        }
    } while(0);
#endif

    return true;
}

static int initICM20600 ()
{
    myImuAux.initialize();
    myImuAux.reset();
    //after reset the range will be set default (+-2G)
    //equivalent to: myImuAux.setAccScaleRange(RANGE_2G);

    //TODO Set the rate sampling to 10 Hz

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

void dumpIMU(bool isConsole)
{
    MYTRACE("IMU : accX %f mg\r\n", myImu.accX());
    MYTRACE("IMU : accY %f mg\r\n", myImu.accY());
    MYTRACE("IMU : accZ %f mg\r\n", myImu.accZ());
    MYTRACE("IMU : gyrX %f dps\r\n", myImu.gyrX());
    MYTRACE("IMU : gyrY %f dps\r\n", myImu.gyrY());
    MYTRACE("IMU : gyrZ %f dps\r\n", myImu.gyrZ());
    MYTRACE("IMU : magX %f uT\r\n", myImu.magX());
    MYTRACE("IMU : magY %f uT\r\n", myImu.magY());
    MYTRACE("IMU : magZ %f uT\r\n", myImu.magZ());
    MYTRACE("IMU : temp %f C \r\n", myImu.temp());
    MYTRACE("IMU : acc2X %d mg\r\n", myImuAux.getAccelerationX());
    MYTRACE("IMU : acc2Y %d mg\r\n", myImuAux.getAccelerationY());
    MYTRACE("IMU : acc2Z %d mg\r\n", myImuAux.getAccelerationZ());
    MYTRACE("IMU : dmp_error: %d\r\n", dmp_error);
}

void loopIMU ()
{
    fxtm_txheader_t* txh = fxtm_gettxheader();

    if (myImu.dataReady())
    {
        ICM_20948_AGMT_t agmt = myImu.getAGMT();

        imuraw_t a[] = {agmt.acc.axes.x, agmt.acc.axes.y, agmt.acc.axes.z};
        imuraw_t m[] = {agmt.mag.axes.x, agmt.mag.axes.y, agmt.mag.axes.z};
        imuraw_t g[] = {agmt.gyr.axes.x, agmt.gyr.axes.y, agmt.gyr.axes.z};

        fxtm_setimu(a, m, g);
        fxstatus_setacc(a);

        txh->temperature3 = myImu.temp();

#ifdef IMU_DMP
        if (dmp_error == 0)
        {
            icm_20948_DMP_data_t data;
            myImu.readDMPdataFromFIFO(&data);
            // Was valid data available?
            if ((myImu.status == ICM_20948_Stat_Ok) ||
                    (myImu.status == ICM_20948_Stat_FIFOMoreDataAvail))
            {
                // We have asked for orientation data so we should receive Quat9
                if ((data.header & DMP_header_bitmap_Quat9) > 0)
                {
                    // Q0 value is computed from this equation: Q0^2 + Q1^2 + Q2^2 + Q3^2 = 1.
                    // In case of drift, the sum will not add to 1, therefore,
                    // quaternion data need to be corrected with right bias values.
                    // The quaternion data is scaled by 2^30.

                    // Scale to +/- 1
                    // Convert to double. Divide by 2^30
                    txh->q1 = ((double)data.Quat9.Data.Q1) / 1073741824.0;
                    txh->q2 = ((double)data.Quat9.Data.Q2) / 1073741824.0;
                    txh->q3 = ((double)data.Quat9.Data.Q3) / 1073741824.0;
                    txh->q0 = sqrt(1.0 - ((txh->q1 * txh->q1) +
                                          (txh->q2 * txh->q2) +
                                          (txh->q3 * txh->q3)
                                  ));
                }
            }
        }
#endif
    } else {
        fxtm_seterror(FXTM_IMU);
    }

    imuraw_t a2[] = {
        myImuAux.getRawAccelerationX(),
        myImuAux.getRawAccelerationY(),
        myImuAux.getRawAccelerationZ()
    };

    fxtm_setimu2(a2);
    txh->gyro2X = myImuAux.getGyroscopeX();
    txh->gyro2Y = myImuAux.getGyroscopeY();
    txh->gyro2Z = myImuAux.getGyroscopeZ();
}
