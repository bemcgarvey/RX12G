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

GyroMode currentGyroMode;
TaskHandle_t gyroTaskHandle;

volatile uint16_t rawServoPositions[MAX_CHANNELS];

void gyroTask(void *pvParameters) {
    currentGyroMode = GYRO_MODE_NORMAL; //TODO use channel data to set this or settings
    while (1) {
        if (currentGyroMode == GYRO_MODE_OFF || GYRO_MODE_NORMAL) {
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
