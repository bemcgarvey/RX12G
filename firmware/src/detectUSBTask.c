/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: detectUSBTask.c                           //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: check for USB connect              //
/////////////////////////////////////////////////////


#include "definitions.h"
#include "detectUSBTask.h"
#include "rtosHandles.h"
#include "tasks.h"
#include "usbapp.h"

TaskHandle_t detectUSBTaskHandle;

void detectUSBTask(void *pvParameters) {
    while (1) {
        U1PWRCbits.USBPWR = 1;
        vTaskDelay(30);
        if (U1OTGSTATbits.VBUSVD == 1) {
            xTaskCreate(_USB_DEVICE_Tasks, "USB_DEVICE_TASKS", 1024, NULL, USB_DEVICE_TASK_PRIORITY, NULL);
            xTaskCreate(USBAppTasks, "USBAppTasks", 1024, NULL, USB_APP_TASK_PRIORITY, &usbAppTaskHandle);
            vTaskSuspend(NULL);
        } else {
            U1PWRCbits.USBPWR = 0;
        }
        vTaskDelay(1000);
    }
}
