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
#include "crsf.h"

int startMode = START_NORMAL;

void _USB_DEVICE_Tasks(void *pvParameters) {
    while (1) {
        /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void initQueues(void) {
    if (settings.satType == SAT_TYPE_SBUS) {
        rxQueue = xQueueCreate(6, 25);
    } else if (settings.satType == SAT_TYPE_CRSF) {
        rxQueue = xQueueCreate(6, CHANNEL_PACKET_LEN);
    } else {
        rxQueue = xQueueCreate(6, 16);
    }
    imuQueue = xQueueCreate(1, 12);
}

void initTasks(void) {
    xTaskCreate(buttonTask, "buttonTask", 128, NULL, BUTTON_TASK_PRIORITY, &buttonTaskHandle);
    xTaskCreate(rxTask, "rxTask", 128, NULL, RX_TASK_PRIORITY, &rxTaskHandle);
    if (settings.rxOnly == RX_ONLY_MODE) {
        xTaskCreate(rxOnlyTask, "rxOnlyTask", 128, NULL, RX_ONLY_TASK_PRIORITY, &rxOnlyTaskHandle);
    } else {
        xTaskCreate(gyroTask, "gyroTask", 4096, NULL, GYRO_TASK_PRIORITY, &gyroTaskHandle);
    }
    xTaskCreate(statusLedTask, "statusLedTask", 128, NULL, STATUS_LED_TASK_PRIORITY, &statusLedTaskHandle);
    if (settings.rxOnly != RX_ONLY_MODE) {
        //Start at priority 1 but will increase to IMU_TASK_PRIORITY after successful initialization
        xTaskCreate(imuTask, "imuTask", 256, NULL, 1, &imuTaskHandle);
    }
    xTaskCreate(detectUSBTask, "detectUSBTask", 128, NULL, DETECT_USB_TASK_PRIORITY, &detectUSBTaskHandle);
    xTaskCreate(usbBindTask, "usbBindTask", 128, NULL, USB_BIND_TASK_PRIORITY, &usbBindTaskHandle);
    //Create this task suspended - will be resumed when needed and then will suspend itself when done
    vTaskSuspend(usbBindTaskHandle);
    vTaskStartScheduler();
}

