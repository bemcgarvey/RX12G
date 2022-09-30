/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: rxTask.c                                  //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: packet rx rtos task                //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "rxTask.h"
#include "output.h"
#include "timers.h"
#include "rtosHandles.h"
#include "gyroTask.h"

TaskHandle_t rxTaskHandle;
QueueHandle_t rxQueue;
uint16_t channelCenters[MAX_CHANNELS];
int centeringCount = 9;

void rxTask(void *pvParameters) {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        rawServoPositions[i] = 0xffff;
        channelCenters[i] = 0;
    }
    while (1) {
        uint16_t buffer[8];
        BaseType_t result;
        uint8_t phase;
        result = xQueueReceive(rxQueue, buffer, portMAX_DELAY);
        if (result) {
            phase = 0;
            for (int i = 1; i < 8; ++i) {
                uint16_t channelData = (buffer[i] >> 8) | (buffer[i] << 8);  //change to little endian
                uint16_t channel = (channelData >> 11) & 0x000f;
                if (i == 1 && (channelData & 0x8000)) { //phase bit is on first servo value
                    phase = 1;
                }
                if (channel < 12) { //Normal channel
                    rawServoPositions[channel] = channelData & 0x07ff;
                } else if (channel == 12) { //XPlus channels
                    channel += (channelData >> 9) & 0x0003;
                    if (phase) { //phase 1 has XPlus 17 - 20, phase 0 has 13 - 16
                        channel += 4;
                    }
                    rawServoPositions[channel] = (channelData & 0x01ff) << 2;
                }
            }
            if (centeringCount != 0) {
                --centeringCount;
                if (centeringCount == 0) {
                    for (int i = 0; i < MAX_CHANNELS; ++i) {
                        channelCenters[i] = rawServoPositions[i];
                    }
                }
            }
        }
    }
}