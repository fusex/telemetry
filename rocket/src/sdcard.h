/*
 * =====================================================================================
 *
 *       Filename:  sdcard.h
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

#ifndef _SDCARD_H
#define _SDCARD_H

void setupSdcard();
void loopSdcard();

// Number of data records in a block.
const uint16_t DATA_DIM = (512 - 4)/sizeof(fxtm_data_t);

//Compute fill so block size is 512 bytes.  FILL_DIM may be zero.
const uint16_t FILL_DIM = 512 - 4 - DATA_DIM*sizeof(fxtm_data_t);

struct block_t {
  uint16_t     count;
  fxtm_data_t  data;
  uint8_t      fill[FILL_DIM];
};
#endif //_SDCARD_H
