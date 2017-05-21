#include "common.h"
#include "imu.h"
#include "radio.h"
#include "gps.h"
#include "sdcard.h"
#include "propellant.h"

void setup()
{
  setupCommon();
  setupRadio();
  setupImu();
  setupGps();
  setupPropellant();
  setupSdcard();
}

void loop()
{
  loopRadio();
  loopImu();
  loopGps();
  loopPropellant();
  loopSdcard();
}

