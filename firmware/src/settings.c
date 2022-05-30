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
#define PRESET_ADDRESS      0x200

Settings settings;
uint16_t channelPresets[MAX_CHANNELS];

uint32_t calculateCRC(void *data, int len) {
    uint32_t crc = 0x12345678;
    uint8_t *p = (uint8_t *)data;
    for (int i = 0; i < len; ++i) {
        crc <<= 1;
        crc ^= *p;
        ++p;
    }
    return crc;
}

bool loadSettings(void) {
    uint32_t savedCrc;
    uint32_t *pData = (uint32_t *) &settings;
    uint32_t address = SETTINGS_ADDRESS;
    for (int i = 0; i < (sizeof (settings) / sizeof (uint32_t)); ++i) {
        if (!EEPROM_WordRead(address, pData)) {
            return false;
        };
        ++pData;
        address += sizeof (uint32_t);
    }
    if (!EEPROM_WordRead(address, &savedCrc)) {
        return false;
    };
    uint32_t crc = calculateCRC(&settings, sizeof(settings));
    
    if ((savedCrc ^ crc) != 0) {
        return false;
    }
    return true;
}

bool saveSettings(void) {
    uint32_t *pData = (uint32_t *) &settings;
    uint32_t address = SETTINGS_ADDRESS;
    for (int i = 0; i < (sizeof (settings) / sizeof (uint32_t)); ++i) {
        if (!EEPROM_WordWrite(address, *pData)) {
            return false;
        };
        ++pData;
        address += sizeof (uint32_t);
    }
    uint32_t crc = calculateCRC(&settings, sizeof(settings));
    if (!EEPROM_WordWrite(address, crc)) {
        return false;
    };
    return true;
}

void loadDefaultSettings(void) {
    settings.outputHz = 50;
    settings.sBusPeriodMs = 7;
    settings.numSBusOutputs = 0;
    settings.failsafeType = NORMAL_FAILSAFE;
}

bool loadPresets(void) {
    uint32_t address = PRESET_ADDRESS;
    for (int i = 0; i < MAX_CHANNELS; i += 2) {
        if (!EEPROM_WordRead(address, (uint32_t *)&channelPresets[i])) {
            return false;
        }
        address += sizeof(uint32_t);
    }
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (channelPresets[i] > 2047) { //if out of bounds set to off
            channelPresets[i] = 0xffff;
        }
    }
    return true;
}
    
bool savePresets(void) {
    uint32_t address = PRESET_ADDRESS;
    for (int i = 0; i < MAX_CHANNELS; i += 2) {
        if (!EEPROM_WordWrite(address, *(uint32_t *)&channelPresets[i])) {
            return false;
        }
        address += sizeof(uint32_t);
    }
    return true;
}