#include "ICM20600.h"
#include <Wire.h>

ICM20600 icm20600(false);

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    Serial2.begin(115200);
    icm20600.initialize();
    icm20600.reset();
}

void loop() {
    Serial2.print("DeviceID: ");
    Serial2.println(icm20600.getDeviceID(), HEX);

    Serial2.print("x = ");
    Serial2.print(icm20600.getGyroscopeX());
    Serial2.println(" dps");
    Serial2.print("y = ");
    Serial2.print(icm20600.getGyroscopeY());
    Serial2.println(" dps");
    Serial2.print("z = ");
    Serial2.print(icm20600.getGyroscopeZ());
    Serial2.println(" dps");


    Serial2.print("x = ");
    Serial2.print(icm20600.getAccelerationX());
    Serial2.println(" mg");
    Serial2.print("y = ");
    Serial2.print(icm20600.getAccelerationY());
    Serial2.println(" mg");
    Serial2.print("z = ");
    Serial2.print(icm20600.getAccelerationZ());
    Serial2.println(" mg");

    Serial2.println("-------------------------");
    delay(600);
}
