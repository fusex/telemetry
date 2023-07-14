#ifndef _FLASH_H
#define _FLASH_H

void setupFlash();
void loopFlash();
void dumpFlash(bool isConsole);
void eraseFlash(uint32_t address);
void readFlash(bool isConsole, uint32_t address);
void writeFlash(uint32_t addr, uint32_t value);
void skipFlash();
void setupOnBootFlash();
void setupFlashSlice();

#endif //_FLASH_H

