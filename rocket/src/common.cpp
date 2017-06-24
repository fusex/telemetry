/*
 * =====================================================================================
 *
 *       Filename:  common.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:44:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#define TAG "COMMON"

#include <fusexutil.h>
#include <Arduino.h>

#include "trame.h"

void setupCommon()
{
    randomSeed(analogRead(0));
    DEBUGDevice.begin(9600);

#ifndef CONFIG_PROD
    while (!DEBUGDevice) ; // Wait for DEBUG serial port to be available
#endif

    TTRACE("init Done\r\n");
}
