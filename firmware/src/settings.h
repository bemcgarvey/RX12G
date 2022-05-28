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
    typedef struct {
        uint32_t outputHz;
        uint32_t sBusPeriodMs;
        uint32_t numSBusOutputs;
        uint32_t failsafeType;
    } Settings;

    extern uint16_t channelPresets[MAX_CHANNELS];
    
    extern Settings settings;
    
    uint32_t calculateCRC(void *data, int len);
    bool loadSettings(void);
    bool saveSettings(void);
    bool loadPresets(void);
    bool savePresets(void);
    void loadDefaultSettings(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */

