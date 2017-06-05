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

#include <fusexconfig.h>
#include <fusexutil.h>

#include <SPI.h>
#include <RH_RF95.h>

#include "trame.h"

RH_RF95 rf95;

extern block_t fxtmblock;

static boolean setSpreadingFactor(byte SF)
{
    // abort and return FALSE if new spreading factor not in acceptable range;
    // otherwise, set spreading factor and return TRUE
    uint8_t currentSF, newLowerNibble, newUpperNibble, current_register_1E_value, new_register_1E_value;

    if ((SF >= 6) && (SF <= 12)) {
	// only set if the spreading factor is in the proper range
	current_register_1E_value = rf95.spiRead(0x1E);

	DTTRACE("Current value of RF95 register 0x1E = 0x%x\r\n",current_register_1E_value);

	//upper nibble of register 0x1E
	currentSF = current_register_1E_value >> 4;
	DTTRACE("Current spreading factor = 0x%x\r\n", current_register_1E_value);

	//same as old lower nibble
	newLowerNibble = (current_register_1E_value & 0x0F);

	// upper nibble equals new spreading factor
	newUpperNibble = (SF << 4);
	new_register_1E_value = (newUpperNibble + newLowerNibble);
	rf95.spiWrite(0x1E, new_register_1E_value);

	DTTRACE("New spreading factor = %d, New value of register 0x1E = %x\r\n",
		SF, new_register_1E_value);

	return true;
    }

    return false;
}

void setupRadio()
{
    if (!rf95.init()) {
	TTRACE("Radio init failed\n\r");
	while(1);
    } else
	TTRACE("Radio init Done\n\r");

    rf95.setTxPower(20,false);
    rf95.setFrequency(869.4);
    rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);

#if 0
    setSpreadingFactor(10);
#elif 1
    setSpreadingFactor(9);
#endif

#if 0
    RH_RF95::ModemConfig modem_config = {
	0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
	0xc4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
	0x0c  // Reg 0x26: LowDataRate=On, Agc=On
    };
    rf95.setModemRegisters(&modem_config);
#endif

//SF 6 does not works properly
//SF 7 8 works properly
//SF 9 work with errors for 125khz and properly for 500khz!
//SF 10 does not works properly for 125khz, and properly for 500khz
//SF 11 does not works properly for 125khz, and works with errors for 500khz
//SF 12 does not works properly for either 125khz and 500khz
}

void loopRadio()
{
    unsigned int packetnbr = CONFIG_PACKETNUMBER;
    unsigned int count     = packetnbr;

    fxtm_data_t* fxtmdata = (fxtm_data_t*) &fxtmblock;

    WTTRACE("b######################################\n\r");

    while (count--){
	uint32_t time = micros();

	rf95.send((uint8_t*)fxtmdata, sizeof(fxtm_data_t));

	WTTRACE("packet sent to in :%ld\r\n", micros()-time);
    }

    //rf95.setModeIdle(); //TODO:

    DTTRACE("Transfer of %d/%d to peer finished!\n\r", packetnbr-count-1, packetnbr);
    WTTRACE("e######################################\n\r");
}
