/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: settings.c                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: rx and gyro settings               //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "settings.h"

#define SETTINGS_ADDRESS    0

Settings settings;

bool loadSettings(void) {
    uint32_t crc = 0x12345678;
    uint32_t savedCrc;
    uint32_t *pData = (uint32_t *) & settings;
    uint32_t address = SETTINGS_ADDRESS;
    for (int i = 0; i < sizeof (settings) / sizeof (uint32_t); ++i) {
        if (!EEPROM_WordRead(address, pData)) {
            return false;
        };
        crc ^= *pData;
        ++pData;
        address += sizeof (uint32_t);
    }
    if (!EEPROM_WordRead(address, &savedCrc)) {
        return false;
    };
    if ((savedCrc ^ crc) != 0) {
        return false;
    }
    return true;
}

bool saveSettings(void) {
    uint32_t crc = 0x12345678;
    uint32_t *pData = (uint32_t *) & settings;
    uint32_t address = SETTINGS_ADDRESS;
    for (int i = 0; i < sizeof (settings) / sizeof (uint32_t); ++i) {
        if (!EEPROM_WordWrite(address, *pData)) {
            return false;
        };
        crc ^= *pData;
        ++pData;
        address += sizeof (uint32_t);
    }
    if (!EEPROM_WordWrite(address, crc)) {
        return false;
    };
    return true;
}

void loadDefaultSettings(void) {
    settings.numSBusOutputs = 0;
    settings.outputHz = 50;
    settings.sBusPeriodMs = 7;
    settings.numSBusOutputs = 0;
    settings.failsafeType = NORMAL_FAILSAFE;
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        settings.channelPresets[i] = 0xffff;
    }
    settings.filler = 0;
}
