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
void RTC_ResetResetID();
void RTC_SetResetID();
uint32_t RTC_GetBootID();
uint16_t RTC_GetResetID();
void RTC_DumpDebug(bool isConsole);

#endif //define _RTC_H
