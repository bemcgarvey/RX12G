/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: gyroTask.c                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: gyro and stability code            //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "gyroTask.h"
#include "rtosHandles.h"
#include "output.h"
#include "tasks.h"

FlightModeType currentFlightMode;
TaskHandle_t gyroTaskHandle;

volatile uint16_t rawServoPositions[MAX_CHANNELS];

void gyroTask(void *pvParameters) {
    //TODO we probably need a FP context here
    currentFlightMode = NORMAL_MODE; //TODO use channel data to set this or settings
    while (1) {
        if (currentFlightMode == OFF_MODE || startMode == START_USB) {
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        } else if (currentFlightMode == NORMAL_MODE) {
            //TODO update channels here
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        } else if (currentFlightMode == AUTO_LEVEL_MODE) {
            
        } else if (currentFlightMode == ATTITUDE_LOCK_MODE) {
            
        } else if (currentFlightMode == LAUNCH_ASSIST_MODE) {
            
        } else {
            //Mode is unrecognized so same as off
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        }
        vTaskDelay(5);
        //TODO check stack level
        /*int stack = uxTaskGetStackHighWaterMark(NULL);
        if (stack < 1) {
            while (1);
        }*/
    }
}
