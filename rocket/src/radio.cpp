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

#include <BGC_Lora.h> 

#include "trame.h"
#include "init.h"

static BGC_Lora lora(BGC_LORA_SS, true); 

void setupRadio()
{
    if (!lora.init()) {
        TTRACE("init failed ! Fatal !!!\n\r");
        Init_SetFatal();
        return;
    } else {
        TTRACE("init Done\n\r");
    }
}

void loopRadio()
{
    unsigned int packetnbr = CONFIG_PACKETNUMBER;
    unsigned int count     = packetnbr;

    WTTRACE("b######################################\n\r");

    while (count--)
    {
        uint32_t time = micros();

        lora.send((uint8_t *)fxtm_getdata(), fxtm_getdatasize());
        WTTRACE("packet sent to in :%ld\r\n", micros() - time);
    }

    DTTRACE("Transfer of %d/%d to peer finished!\n\r", packetnbr-count-1, packetnbr);
    WTTRACE("e######################################\n\r");
}
