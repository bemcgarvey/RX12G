/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: crsfTelemetryTask.c                       //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: ExpressLRS telemetry               //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "crsfTelemetryTask.h"
#include "rtosHandles.h"
#include "crsf.h"
#include "gyroTask.h"
#include "attitude.h"

TaskHandle_t crsfTelemetryTaskHandle;

void crsfTelemetryTask(void *pvParameters) {
    portTASK_USES_FLOATING_POINT();
    vTaskDelay(1000);
    while (1) {
        while (telemetryBusy()) {
            vTaskDelay(1);
        }
        sendModeTelemetry(currentFlightMode);
        while (telemetryBusy()) {
            vTaskDelay(1);
        }
        sendAttitudeTelemetry(attitude.ypr.roll, attitude.ypr.pitch, attitude.ypr.yaw);
        vTaskDelay(500);
    }
}