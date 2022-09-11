/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: rxOnlyTask.c                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: task for rx functions only (no imu)//
/////////////////////////////////////////////////////

#include "definitions.h"
#include "rxOnlyTask.h"
#include "output.h"
#include "gyroTask.h"
#include "imu.h"

TaskHandle_t rxOnlyTaskHandle;

void rxOnlyTask(void *pvParameters) {
    currentFlightMode = OFF_MODE;
    WDT_Enable();
    while (1) {
        if (needToUpdateOutputs) {
            needToUpdateOutputs = false;
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        }
        WDT_Clear();
        vTaskDelay(1);
    }
}