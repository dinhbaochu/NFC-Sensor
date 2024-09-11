#ifndef NTAG_H
#define NTAG_H

#include "Arduino.h"

class Ntag {
public:
    enum REGISTER_NR {
    };
    Ntag();

    bool begin();
    bool writeEeprom(word address, byte* pdata, byte length);
    bool readRegister(REGISTER_NR regAddr, byte& value);
    bool writeRegister(REGISTER_NR regAddr, byte mask, byte regdat);

private:
    enum BLOCK_TYPE {
        CONFIG = 0x1,
        USERMEM = 0x2,
        REGISTER = 0x4,
        SRAM = 0x8
    };

    static const byte NTAG_BLOCK_SIZE = 16;
    static const word EEPROM_BASE_ADDR = 0x10;

    bool write(BLOCK_TYPE bt, word byteAddress, byte* pdata, byte length);
    bool readBlock(BLOCK_TYPE bt, byte memBlockAddress, byte* p_data, byte data_size);
    bool writeBlock(BLOCK_TYPE bt, byte memBlockAddress, byte* p_data);
    bool writeBlockAddress(BLOCK_TYPE dt, byte addr);
    bool end_transmission();
    bool _triggered;
};

#endif // NTAG_H

