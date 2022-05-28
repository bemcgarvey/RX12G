/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: settings.h                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: rx and gyro settings               //
/////////////////////////////////////////////////////

#ifndef SETTINGS_H
#define	SETTINGS_H

#include "output.h"

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct __attribute__((packed)) {
        uint16_t outputHz;
        uint16_t channelPresets[MAX_CHANNELS];
        uint8_t sBusPeriodMs;
        uint8_t numSBusOutputs;
        uint8_t failsafeType;
    } Settings;

    extern Settings settings;
    
    uint32_t calculateCRC(void *data, int len);
    bool loadSettings(void);
    bool saveSettings(void);
    void loadDefaultSettings(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */

