#include "tagwrite.h"
#include "Wire.h"

#define HWire Wire

Ntag::Ntag() :
    _triggered(false)
{
}

bool Ntag::begin() {
    bool bResult = true;
    HWire.begin();
#ifndef ARDUINO_SAM_DUE
    HWire.beginTransmission(0x55);
    bResult = HWire.endTransmission() == 0;
#else
    return true;
#endif

    return bResult;
}
bool Ntag::writeEeprom(word address, byte* pdata, byte length) {
    byte blockNr = (address + EEPROM_BASE_ADDR) / NTAG_BLOCK_SIZE;
    byte offset = (address + EEPROM_BASE_ADDR) % NTAG_BLOCK_SIZE;
    byte remaining = length;
    byte buffer[NTAG_BLOCK_SIZE];

    // Handle unaligned initial block
    if (offset > 0) {
        byte readSize = NTAG_BLOCK_SIZE - offset;
        if (!readBlock(USERMEM, blockNr, buffer, NTAG_BLOCK_SIZE)) {
            return false;
        }
        byte copySize = (remaining < readSize) ? remaining : readSize;
        memcpy(buffer + offset, pdata, copySize);
        if (!writeBlock(USERMEM, blockNr, buffer)) {
            return false;
        }
        remaining -= copySize;
        pdata += copySize;
        blockNr++;
    }

    // Handle aligned blocks
    while (remaining > 0) {
        byte writeSize = (remaining < NTAG_BLOCK_SIZE) ? remaining : NTAG_BLOCK_SIZE;
        if (writeSize < NTAG_BLOCK_SIZE) {
            if (!readBlock(USERMEM, blockNr, buffer, NTAG_BLOCK_SIZE)) {
                return false;
            }
            memcpy(buffer, pdata, writeSize);
            if (!writeBlock(USERMEM, blockNr, buffer)) {
                return false;
            }
        } else {
            if (!writeBlock(USERMEM, blockNr, pdata)) {
                return false;
            }
        }
        remaining -= writeSize;
        pdata += writeSize;
        blockNr++;
    }

    return true;
}

bool Ntag::writeBlock(BLOCK_TYPE bt, byte memBlockAddress, byte* p_data) {
    if (!writeBlockAddress(bt, memBlockAddress)) {
        return false;
    }
    for (int i = 0; i < NTAG_BLOCK_SIZE; i++) {
        HWire.write(p_data[i]);
    }
    if (!end_transmission()) {
        return false;
    }
    switch (bt) {
    case CONFIG:
    case USERMEM:
        delay(5); // 16 bytes (one block) written in 4.5 ms (EEPROM)
        break;
    case REGISTER:
    case SRAM:
        delayMicroseconds(500); // 0.4 ms (SRAM - Pass-through mode) including all overhead
        break;
    }
    return true;
}

bool Ntag::readBlock(BLOCK_TYPE bt, byte memBlockAddress, byte* p_data, byte data_size) {
    if (data_size > NTAG_BLOCK_SIZE || !writeBlockAddress(bt, memBlockAddress)) {
        return false;
    }
    if (!end_transmission()) {
        return false;
    }
    if (HWire.requestFrom(0x55, data_size) != data_size) {
        return false;
    }
    for (byte i = 0; i < data_size; i++) {
        if (HWire.available()) {
            p_data[i] = HWire.read();
        }
    }
    return true;
}

bool Ntag::writeBlockAddress(BLOCK_TYPE dt, byte addr) {
   
    HWire.beginTransmission(0x55);
    HWire.write(addr);
    return true;
}

bool Ntag::end_transmission() {
    return HWire.endTransmission() == 0;
}


