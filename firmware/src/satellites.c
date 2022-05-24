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

static bool detectedSatellites[3];
TaskHandle_t satLedTaskHandle;

enum {
    DSMX_EXTERNAL_11MS = 10, DSMX_INTERNAL_11MS = 9
};

void initSatellites(void) {
    satPowerOn(true);
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
    initUARTs(detectedSatellites);
}

void satPowerOn(bool powerOn) {
    if (powerOn) {
        SAT_POWER_Set();
    } else {
        SAT_POWER_Clear();
    }
}

bool bindSats(void) {
    disableUARTs();
    satPowerOn(false);
    CORETIMER_DelayMs(500);
    LATBbits.LATB0 = 1;
    LATCbits.LATC15 = 1;
    LATAbits.LATA11 = 1;
    TRISBbits.TRISB0 = 0;
    TRISCbits.TRISC15 = 0;
    TRISAbits.TRISA11 = 0;
    satPowerOn(true);
    CORETIMER_DelayMs(70);
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

void satLedTask(void *pvParameters) {
    bool activity;
    while (1) {
        activity = false;
        uint32_t time = getSystemTime();
        if (time - lastRxTime[SAT1] < 100) {
            SAT1_LED_Set();
            activity = true;
            failsafeEngaged = false;
        } else {
            SAT1_LED_Clear();
        }
        if (time - lastRxTime[SAT2] < 100) {
            SAT2_LED_Set();
            activity = true;
            failsafeEngaged = false;
        } else {
            SAT2_LED_Clear();
        }
        if (time - lastRxTime[SAT3] < 100) {
            SAT3_LED_Set();
            activity = true;
            failsafeEngaged = false;
        } else {
            SAT3_LED_Clear();
        }
        if (!activity) {
            if (!failsafeEngaged) {
                engageFailsafe();
            }
        }
        vTaskDelay(100);
    }
}