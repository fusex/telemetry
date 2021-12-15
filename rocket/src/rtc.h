/*
 * =====================================================================================
 *
 *       Filename:  gps.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:31:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */
#ifndef _RTC_H
#define _RTC_H

void setupRTC();
void loopRTC();
void setResetResetID();
void setResetID();
uint32_t getBootID();
uint16_t getResetID();

#endif //define _RTC_H
