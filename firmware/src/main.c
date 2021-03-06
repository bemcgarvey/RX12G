/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: main.c                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: main                               //
/////////////////////////////////////////////////////

#include "definitions.h"  
#include "timers.h"
#include "satellites.h"
#include "output.h"
#include "tasks.h"
#include "settings.h"
#include "imu.h"

//TODO add clock monitoring and NMI handler
//TODO enable BOR and do fast start when detected
//BUG when switching to bootloader mode servos sometimes go way out of range

int main(void) {
    if (RCONbits.POR == 1) {
        //power on reset
        RCONbits.POR = 0;
        RCONbits.BOR = 0;
        startMode = START_NORMAL;
    } else if (RCONbits.WDTO == 1) {
        startMode = START_WDTO;
        RCONbits.WDTO = 0;
    } else if (RCONbits.BOR == 1) {
        startMode = START_WDTO; //Treat a brown-out like a WDT reset - start quickly
        RCONbits.BOR = 0;
    }
    SYS_Initialize(NULL);
    if (!loadSettings()) {
        loadDefaultSettings();
        saveSettings();
    }
    if (settings.failsafeType == PRESET_FAILSAFE) {
        loadPresets();
    }
    if (startMode != START_WDTO) {
        CORETIMER_DelayMs(30);
        if (U1OTGSTATbits.VBUSVD == 1) {
            startMode = START_USB;
        } else {
            U1PWRCbits.USBPWR = 0;
        }
    } else {
        U1PWRCbits.USBPWR = 0;
    }
    initQueues();
    startSystemTime();
    initSatellites();
    initOutputs();
    if (initIMU()) {
        imuInitialized = true;
    } else {
        imuInitialized = false;
    }
    //Lock PPS
    __builtin_disable_interrupts();
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCONbits.IOLOCK = 1;
    __builtin_enable_interrupts();
    initTasks(); //Starts rtos tasks - Does not return
    while (true) {
        /* Execution should not come here during normal operation */
    }
    return ( EXIT_FAILURE);
}

/*******************************************************************************
 End of File
 */

