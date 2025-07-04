/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: satellites.c                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: satellite rx functions             //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "satellites.h"
#include "uart.h"
#include "timers.h"
#include "output.h"
#include "tasks.h"
#include "settings.h"
#include "crsf.h"

static bool detectedSatellites[3];

enum {
    DSMX_EXTERNAL_11MS = 10, DSMX_INTERNAL_11MS = 9
};

void initSatellites(void) {
    satPowerOn(true);
    if (startMode == START_WDTO) {
        //Just assume we have all sats so we can start quickly
        detectedSatellites[SAT1] = true;
        detectedSatellites[SAT2] = true;
        detectedSatellites[SAT3] = true;
    } else if (settings.satType == SAT_TYPE_SBUS) {
        detectedSatellites[SAT1] = false;
        detectedSatellites[SAT2] = false;
        detectedSatellites[SAT3] = false;
        CNCONAbits.ON = 1;
        CNCONBbits.ON = 1;
        CNCONCbits.ON = 1;
        CNPUBbits.CNPUB0 = 1;
        CNPUCbits.CNPUC15 = 1;
        CNPUAbits.CNPUA11 = 1;
        CORETIMER_DelayMs(1);
        CNCONAbits.EDGEDETECT = 1;
        CNCONBbits.EDGEDETECT = 1;
        CNCONCbits.EDGEDETECT = 1;
        CNNEBbits.CNNEB0 = 1;
        CNENBbits.CNIEB0 = 1;
        CNNECbits.CNNEC15 = 1;
        CNENCbits.CNIEC15 = 1;
        CNNEAbits.CNNEA11 = 1;
        CNENAbits.CNIEA11 = 1;
        int p = PORTA;
        p = PORTB;
        p = PORTC;
        (void) p;
        CORETIMER_DelayMs(1500);
        if (SAT1_RX_Get() == 0 || CNSTATBbits.CNSTATB0 == 1) {
            detectedSatellites[SAT1] = true;
        }
        if (SAT2_RX_Get() == 0 || CNSTATCbits.CNSTATC15 == 1) {
            detectedSatellites[SAT2] = true;
        }
        if (SAT3_RX_Get() == 0 || CNSTATAbits.CNSTATA11 == 1) {
            detectedSatellites[SAT3] = true;
        }
        CNCONAbits.ON = 0;
        CNPUAbits.CNPUA11 = 0;
        CNCONBbits.ON = 0;
        CNPUBbits.CNPUB0 = 0;
        CNCONCbits.ON = 0;
        CNPUCbits.CNPUC15 = 0;
    } else if (settings.satType == SAT_TYPE_CRSF) {
        detectedSatellites[SAT1] = false;
        detectedSatellites[SAT2] = true;
        detectedSatellites[SAT3] = false;
        satPowerOn(false);  //ELRS rx uses 5V from servo pins
    } else {
        detectedSatellites[SAT1] = false;
        detectedSatellites[SAT2] = false;
        detectedSatellites[SAT3] = false;
        CNPDBbits.CNPDB0 = 1;
        CNPDCbits.CNPDC15 = 1;
        CNPDAbits.CNPDA11 = 1;
        CORETIMER_DelayMs(50);
        if (SAT1_RX_Get() == 1) {
            detectedSatellites[SAT1] = true;
        }
        if (SAT2_RX_Get() == 1) {
            detectedSatellites[SAT2] = true;
        }
        if (SAT3_RX_Get() == 1) {
            detectedSatellites[SAT3] = true;
        }
        CNPDBbits.CNPDB0 = 0;
        CNPDCbits.CNPDC15 = 0;
        CNPDAbits.CNPDA11 = 0;
    }
    initUARTs(detectedSatellites);
}

void satPowerOn(bool powerOn) {
    if (powerOn && settings.satType == SAT_TYPE_DSMX) {
        SAT_POWER_Set();
    } else {
        SAT_POWER_Clear();
    }
}

bool bindSats(void) {
    if (settings.satType == SAT_TYPE_SBUS || settings.satType == SAT_TYPE_CRSF) {
        return true;
    }
    disableUARTs();
    satPowerOn(false);
    vTaskDelay(500);
    LATBbits.LATB0 = 1;
    LATCbits.LATC15 = 1;
    LATAbits.LATA11 = 1;
    TRISBbits.TRISB0 = 0;
    TRISCbits.TRISC15 = 0;
    TRISAbits.TRISA11 = 0;
    satPowerOn(true);
    vTaskDelay(70);
    int firstSat = -1;
    int pulseCount = DSMX_EXTERNAL_11MS;
    if (detectedSatellites[SAT1]) {
        firstSat = SAT1;
    } else if (detectedSatellites[SAT2]) {
        firstSat = SAT2;
    } else if (detectedSatellites[SAT3]) {
        firstSat = SAT3;
    }
    for (int i = 0; i < pulseCount; ++i) {
        if (firstSat == SAT1) {
            if (i < pulseCount - 1) {
                LATBbits.LATB0 = 0;
            }
        } else {
            LATBbits.LATB0 = 0;
        }
        if (firstSat == SAT2) {
            if (i < pulseCount - 1) {
                LATCbits.LATC15 = 0;
            }
        } else {
            LATCbits.LATC15 = 0;
        }
        if (firstSat == SAT3) {
            if (i < pulseCount - 1) {
                LATAbits.LATA11 = 0;
            }
        } else {
            LATAbits.LATA11 = 0;
        }
        CORETIMER_DelayUs(118);
        LATBbits.LATB0 = 1;
        LATCbits.LATC15 = 1;
        LATAbits.LATA11 = 1;
        CORETIMER_DelayUs(123);
    }
    TRISBbits.TRISB0 = 1;
    TRISCbits.TRISC15 = 1;
    TRISAbits.TRISA11 = 1;
    initUARTs(detectedSatellites);
    return true;
}
