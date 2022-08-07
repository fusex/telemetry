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
