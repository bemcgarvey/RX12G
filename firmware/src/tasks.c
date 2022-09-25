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
#include "rxTask.h"
#include "gyroTask.h"
#include "statusLedTask.h"
#include "usbapp.h"
#include "imu.h"
#include "rxOnlyTask.h"
#include "detectUSBTask.h"
#include "usbBindTask.h"

int startMode = START_NORMAL;

void _USB_DEVICE_Tasks(void *pvParameters) {
    while (1) {
        /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void initQueues(void) {
    rxQueue = xQueueCreate(6, 16);
    imuQueue = xQueueCreate(1, 12);
}

void initTasks(void) {
    xTaskCreate(buttonTask, "buttonTask", 128, NULL, 4, &buttonTaskHandle);
    xTaskCreate(rxTask, "rxTask", 128, NULL, 3, &rxTaskHandle);
    if (settings.rxOnly == RX_ONLY_MODE) {
        xTaskCreate(rxOnlyTask, "rxOnlyTask", 128, NULL, 2, &rxOnlyTaskHandle);
    } else {
        xTaskCreate(gyroTask, "gyroTask", 4096, NULL, 2, &gyroTaskHandle);
    }
    xTaskCreate(statusLedTask, "statusLedTask", 128, NULL, 1, &statusLedTaskHandle);
    if (imuInitialized) {
        xTaskCreate(imuTask, "imuTask", 256, NULL, 4, &imuTaskHandle);
    }
    xTaskCreate(detectUSBTask, "detectUSBTask", 128, NULL, 1, &detectUSBTaskHandle);
    xTaskCreate(usbBindTask, "usbBindTask", 128, NULL, 1, &usbBindTaskHandle);
    //Create this task suspended - will be resumed when needed and then will suspend itself when done
    vTaskSuspend(usbBindTaskHandle);
    vTaskStartScheduler();
}

