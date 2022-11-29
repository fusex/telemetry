#define TAG "ATMOS"

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
        Init_SetFailed();
        return;
    }

    hdc1080.begin(BGC_I2C_TEMP_HUM_ADDR);
    if (hdc1080.readManufacturerId() != 0x5449 ||
            hdc1080.readDeviceId() != 0x1050) {
        Init_SetFailed();
        return;
    }

    TTRACE("init Done.\r\n");
    module_setup(TAG, FXTM_SUCCESS);
}

void loopAtmos ()
{
#ifdef CONFIG_TEMPERATURE_FROM_BMP
    float temperature = bmp.readTemperature();
#else
    float temperature = hdc1080.readTemperature();
#endif
    float humidity   = hdc1080.readHumidity();
    float pressure   = bmp.readPressure();

    fxtm_settemperature(temperature);
    fxtm_setpressure(pressure);
    fxtm_sethumidity(humidity);
}
