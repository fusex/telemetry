
#define ACCEL_2G_RANGE_COEF 16.384 /* For +-2g */
#define ACCEL_4G_RANGE_COEF  8.192 /* For +-4g */
#define ACCEL_8G_RANGE_COEF  4.096 /* For +-8g */
#define ACCEL_16G_RANGE_COEF 2.048 /* For +-16g */

#define ACCEL_COEF   ACCEL_16G_RANGE_COEF 

void fxreel_getaccel()
{
    fxtm_data_t* tm = &fxtmblock.data;
    imuraw_t a[];

    IMU_SENSOR_GET(accel, tm, a[0], a[1], a[2]);
    ax = ((float)a[0])/ACCEL_COEF;
    ay = ((float)a[1])/ACCEL_COEF;
    az = ((float)a[2])/ACCEL_COEF;
}

void fxreel_getgps (gpsraw_t latitude, gpsraw_t longitude)
{
    fxtm_data_t* tm = &fxtmblock.data;

    fxreel->gps.lat = fxtm->gps.lat/;
    fxreel->gps.lon = fxtm->gps.lon/;
}
