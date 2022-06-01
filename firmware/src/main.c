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
//TODO enable WDT and/or DMT

int main(void) {
    startMode = START_NORMAL;
    SYS_Initialize(NULL);
    if (!loadSettings()) {
        loadDefaultSettings();
        saveSettings();
        SAT2_LED_Set();
        while (1);
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
    if (!initIMU()) {
        //TODO if IMU can't be started we should force off mode and indicate with an led code
        SAT2_LED_Set();
        while (1);
    }
    //TODO lock PPS here since all pins should be assigned.
    initTasks(); //Starts rtos tasks - Does not return
    while (true) {
        /* Execution should not come here during normal operation */
    }
    return ( EXIT_FAILURE);
}

/*******************************************************************************
 End of File
 */

