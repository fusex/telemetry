/*
* =====================================================================================
*
*       Filename:  radioparam.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  17/07/2017 16:52:45
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@derbsellicon.com
*        Company:  Derb.io
*
* =====================================================================================
*/

#ifndef _RADIOPARAM_H
#define _RADIOPARAM_H
#include <RH_RF95.h>

#if 1
#define FX_SF           9
#else
#define FX_SF          10
#endif
//SF 6 does not works properly
//SF 7 8 works properly
//SF 9 work with errors for 125khz and properly for 500khz!
//SF 10 does not works properly for 125khz, and properly for 500khz
//SF 11 does not works properly for 125khz, and works with errors for 500khz
//SF 12 does not works properly for either 125khz and 500khz

#define FX_TXPOWER      20
#define FX_FREQUENCY    869.4
#define FX_MODEMCONFIG  RH_RF95::Bw500Cr45Sf128

RH_RF95::ModemConfig modem_config = {
    0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
    0xc4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
    0x0c  // Reg 0x26: LowDataRate=On, Agc=On
};

#define fx_setModemConfig() { \
    if (0) \
	rf95.setModemRegisters(&modem_config); \
    else \
	rf95.setModemConfig(FX_MODEMCONFIG); \
}

#define fx_setSpreadingFactor() { \
    byte SF = FX_SF; \
    uint8_t currentSF, newLowerNibble, newUpperNibble, cur_reg_val, new_reg_val; \
    if ((SF < 6) || (SF > 12)) { \
	TRACE("Invalid SF :0x%x",SF); \
        fatal(); \
    } else {\
	cur_reg_val = rf95.spiRead(0x1E); \
	currentSF = cur_reg_val >> 4; \
	newLowerNibble = (cur_reg_val & 0x0F); \
	newUpperNibble = (SF << 4); \
	new_reg_val = (newUpperNibble + newLowerNibble); \
	rf95.spiWrite(0x1E, new_reg_val); \
    } \
}

#define fx_setTxPower() { \
    rf95.setTxPower(FX_TXPOWER,false); \
}

#define fx_setFrequency() { \
    rf95.setFrequency(FX_FREQUENCY); \
}

#endif // _RADIOPARAM_H
