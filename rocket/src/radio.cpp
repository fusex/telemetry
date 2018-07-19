/*
 * =====================================================================================
 *
 *       Filename:  radio.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:23:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io
 *
 * =====================================================================================
 */
#define TAG "RADIO"

#include <fusexconfig.h>
#include <fusexutil.h>
#include <radioparam.h>

#include <SPI.h>
#include <RH_RF95.h>

#include "trame.h"
#include "common.h"

RH_RF95 rf95;

void setupRadio()
{
    if (!rf95.init()) {
	TTRACE("init failed ! Fatal !!!\n\r");
	setupSetFatal();
	return;
    } else {
	TTRACE("init Done\n\r");
    }

    fx_setTxPower();
    fx_setFrequency();
    fx_setModemConfig();
    fx_setSpreadingFactor();
}

void loopRadio()
{
    unsigned int packetnbr = CONFIG_PACKETNUMBER;
    unsigned int count     = packetnbr;

    WTTRACE("b######################################\n\r");

    while (count--){
	uint32_t time = micros();

	rf95.send((uint8_t*)fxtm_getdata(), fxtm_getdatasize());
	WTTRACE("packet sent to in :%ld\r\n", micros()-time);
    }

    DTTRACE("Transfer of %d/%d to peer finished!\n\r", packetnbr-count-1, packetnbr);
    WTTRACE("e######################################\n\r");
}
