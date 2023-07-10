#define TAG "ATMOS"

#include <fusexcommon.h>
#include <fusexconfig.h>
#include <fusexutil.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <ClosedCube_HDC1080.h>
#include <BGC_I2c.h>
extern "C" {
#include <trame.h>
}

#include "init.h"

static ClosedCube_HDC1080 hdc1080;
static Adafruit_BMP280    bmp;

void setupAtmos ()
{
    module_add(TAG);

    if (!bmp.begin(BGC_I2C_PRESSURE_ADDR, 0x58)) {
        module_setup(TAG, FXTM_FAILURE);
        Init_SetFailed();
        return;
    }

    hdc1080.begin(BGC_I2C_TEMP_HUM_ADDR);
    if (hdc1080.readManufacturerId() != 0x5449 ||
            hdc1080.readDeviceId() != 0x1050) {
        module_setup(TAG, FXTM_FAILURE);
        Init_SetFailed();
        return;
    }
    hdc1080.setResolution(HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_8BIT);

    module_setup(TAG, FXTM_SUCCESS);
}

void dumpAtmos(bool isConsole)
{
    float bmptemperature  = bmp.readTemperature();
    float pressure        = bmp.readPressure();
    float hdctemperature  = hdc1080.readTemperature();
    float humidity        = hdc1080.readHumidity();
    float altitude        = bmp.readAltitude(PRESSURE_SEALEVEL_REF);

    MYTRACE("ATMOS bmp.temperature: %f C\r\n", bmptemperature);
    MYTRACE("ATMOS hdc.temperature: %f C\r\n", hdctemperature);
    MYTRACE("ATMOS pressure: %f Pa\r\n", pressure);
    MYTRACE("ATMOS humidity: %f %%\r\n", humidity);
    MYTRACE("ATMOS altitude: %f m\r\n", altitude);
}

void loopAtmos ()
{
#ifdef CONFIG_TEMPERATURE_FROM_BMP
    float temperature  = bmp.readTemperature();
    float temperature2 = hdc1080.readTemperature();
#else
    float temperature  = hdc1080.readTemperature();
    float temperature2 = bmp.readTemperature();
#endif
    float humidity   = hdc1080.readHumidity();
    float pressure   = bmp.readPressure();
    float altitude   = bmp.readAltitude(PRESSURE_SEALEVEL_REF);

    fxtm_settemperature(temperature);
    fxtm_setpressure(pressure/100);
    fxtm_sethumidity(humidity);

    fxtm_txheader_t* txh = fxtm_gettxheader();
    txh->altitude     = altitude;
    txh->temperature2 = temperature2;
}
