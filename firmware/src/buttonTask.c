/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: buttonTask.c                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: bind button rtos task              //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "buttonTask.h"
#include "rtosHandles.h"
#include "satellites.h"
#include "uart.h"

TaskHandle_t buttonTaskHandle;

void buttonHandler(EXTERNAL_INT_PIN pin, uintptr_t context) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(buttonTaskHandle, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void buttonTask(void *pvParameters) {
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_3, buttonHandler, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_3);
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(2000);
        if (BIND_BUTTON_Get() == 0) {
            LED_B_Set();
            bindSats();
            while (1) {
                int n = ulTaskNotifyTake(pdTRUE, 100);
                if (n) {
                    satPowerOn(false);
                    vTaskDelay(500);
                    satPowerOn(true);
                    break;
                }
                if (validPacketReceived) {
                    break;
                }
            }
            LED_B_Clear();
        }
    }
}

