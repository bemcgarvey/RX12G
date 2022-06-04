/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: tasks.c                                   //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: task creation and scheduler start  //
/////////////////////////////////////////////////////

#include "configuration.h"
#include "definitions.h"
#include "tasks.h"
#include "buttonTask.h"
#include "rtosHandles.h"
#include "output.h"
#include "rxTask.h"
#include "gyroTask.h"
#include "satellites.h"
#include "statusLedTask.h"
#include "usbapp.h"
#include "imu.h"

QueueHandle_t rxQueue;

int startMode = START_NORMAL;

void _USB_DEVICE_Tasks(void *pvParameters) {
    while (1) {
        /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void initQueues(void) {
    rxQueue = xQueueCreate(6, 16); //TODO find best length for this
}

void initTasks(void) {
    xTaskCreate(buttonTask, "button", 128, NULL, 5, &buttonTaskHandle);
    xTaskCreate(rxTask, "rxtask", 128, NULL, 4, &rxTaskHandle);
    xTaskCreate(gyroTask, "gyroTask", 4096, NULL, 3, &gyroTaskHandle);
    xTaskCreate(statusLedTask, "statusLedTask", 128, NULL, 1, &statusLedTaskHandle);
    xTaskCreate(imuTask, "imuTask", 128, NULL, 5, &imuTaskHandle);
    if (startMode == START_USB) {
        xTaskCreate(_USB_DEVICE_Tasks, "USB_DEVICE_TASKS", 1024, NULL, 1, NULL);
        xTaskCreate(USBAppTasks, "USBAppTasks", 1024, NULL, 1, &usbAppTaskHandle);
    }
    vTaskStartScheduler();
}

