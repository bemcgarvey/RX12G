/* 
 * File:   settings.h
 * Author: bemcg
 *
 * Created on May 24, 2022, 12:39 PM
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include "output.h"

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct {
        uint16_t outputHz;
        uint16_t channelPresets[MAX_CHANNELS];
        uint8_t sBusPeriodMs;
        uint8_t numSBusOutputs;
        uint8_t failsafeType;
        uint8_t filler;
        uint32_t crc;
    } Settings;

    extern Settings settings;
    
    bool loadSettings(void);
    bool saveSettings(void);
    void loadDefaultSettings(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */

