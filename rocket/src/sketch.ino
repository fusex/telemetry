#include "common.h"
#include "imu.h"
#include "radio.h"

void setup()
{
  setupCommon();
  setupImu();
  setupRadio();
}

void loop()
{
  loopImu();
  loopRadio();
}

