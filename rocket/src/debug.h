#ifndef _DEBUG_H
#define _DEBUG_H

void i2c_scanner (bool isConsole);
uint8_t i2c_read (bool isConsole, uint8_t address, uint8_t reg);
void i2c_write (bool isConsole, uint8_t address, uint8_t reg, uint8_t value);

#endif //define _DEBUG_H
