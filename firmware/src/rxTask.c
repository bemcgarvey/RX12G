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

TaskHandle_t rxTaskHandle;
static uint16_t rawServoPositions[MAX_CHANNELS];

void rxTask(void *pvParameters) {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        rawServoPositions[i] = 0xffff;
    }
    while (1) {
        uint16_t buffer[8];
        BaseType_t result;
        result = xQueueReceive(rxQueue, buffer, portMAX_DELAY);
        if (result) {
            for (int i = 1; i < 8; ++i) {
                uint16_t channelData = (buffer[i] >> 8) | (buffer[i] << 8);
                uint16_t channel = (channelData >> 11) & 0x000f;
                if (channel == 12) { //XPlus channels - this is not tested!
                    channel += (channelData >> 9) & 0x0003;
                    if (channelData & 0x80) { //phase = 1
                        channel += 4;
                    }
                    rawServoPositions[channel] = (channelData & 0x01ff) << 2;
                } else { //Normal channel
                    rawServoPositions[channel] = channelData & 0x07ff;
                }
            }
            xQueueOverwrite(rawServoQueue, rawServoPositions);
        }
    }
}