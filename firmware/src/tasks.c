/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: tasks.c                                   //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: task creation and scheduler start  //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "tasks.h"
#include "buttonTask.h"
#include "rtosHandles.h"
#include "output.h"
#include "rxTask.h"
#include "gyroTask.h"
#include "satellites.h"
#include "statusLedTask.h"

QueueHandle_t rxQueue;

void initQueues(void) {
    rxQueue = xQueueCreate(6, 16);  //TODO find best length for this
}

void initTasks(void) {
    xTaskCreate(buttonTask, "button", 128, NULL, 5, &buttonTaskHandle);
    xTaskCreate(rxTask, "rxtask", 128, NULL, 4, &rxTaskHandle);
    xTaskCreate(gyroTask, "gyroTask", 1024, NULL, 2, &gyroTaskHandle);
    xTaskCreate(satLedTask, "satLedTask", 128, NULL, 1, &satLedTaskHandle);
    xTaskCreate(statusLedTask, "statusLedTask", 128, NULL, 1, &statusLedTaskHandle);
    vTaskStartScheduler();
}

