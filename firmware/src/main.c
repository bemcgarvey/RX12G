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

int main(void) {
    startMode = START_NORMAL;
    SYS_Initialize(NULL);
    if (!loadSettings()) {
        loadDefaultSettings();
        saveSettings();
    }
    loadPresets();
    CORETIMER_DelayMs(30);
    if (U1OTGSTATbits.VBUSVD == 1) {
        startMode = START_USB;
    } else {
        U1PWRCbits.USBPWR = 0;
    }
    initQueues();
    startSystemTime();
    initSatellites();
    initOutputs();
    initTasks(); //Starts rtos tasks - Does not return
    while (true) {
        /* Execution should not come here during normal operation */
    }
    return ( EXIT_FAILURE);
}

/*******************************************************************************
 End of File
 */

