#ifndef _FLASH_H
#define _FLASH_H

void setupFlash();
void loopFlash();
void dumpFlash(bool isConsole);
void eraseFlash(uint32_t address);
void readFlash(bool isConsole, uint32_t address);
void setupFlashSlice();

#endif //_FLASH_H

