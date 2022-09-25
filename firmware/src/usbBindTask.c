/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: uabBindTask.c                             //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: bind from usb task                 //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "rtosHandles.h"
#include "usbBindTask.h"
#include "buttonTask.h"
#include "satellites.h"
#include "uart.h"

TaskHandle_t usbBindTaskHandle;

void usbBindTask(void *pvParameters) {
    while (1) {
        isBinding = true;
        bindSats();
        while (isBinding) {
            if (validPacketReceived) {
                isBinding = false;
            }
            vTaskDelay(50);
        }
        vTaskSuspend(NULL);
    }
}