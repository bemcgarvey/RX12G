/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: statusLedTask.c                           //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: status and mode led control        //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "statusLedTask.h"
#include "rtosHandles.h"
#include "output.h"
#include "gyroTask.h"

TaskHandle_t statusLedTaskHandle;

typedef enum {
    LED_OFF = 0, LED_ON, LED_BLINK_SLOW, LED_BLINK_FAST
} LedState;

void statusLedTask(void *pvParameters) {
    LedState LED_A_State = LED_OFF;
    LedState LED_B_State = LED_OFF;
    int blinkCount = 0;
    while (1) {
        switch (currentGyroMode) {
            case GYRO_MODE_OFF:
                LED_A_State = LED_OFF;
                LED_B_State = LED_OFF;
                break;
            case GYRO_MODE_NORMAL:
                LED_A_State = LED_OFF;
                LED_B_State = LED_ON;
                break;
            case GYRO_MODE_LEVEL:
                LED_A_State = LED_ON;
                LED_B_State = LED_OFF;
            case GYRO_MODE_ATTITUDE_LOCK:
                LED_A_State = LED_ON;
                LED_B_State = LED_ON;
            case GYRO_MODE_LAUNCH_ASSIST:
                LED_A_State = LED_OFF;
                LED_B_State = LED_BLINK_SLOW;
                break;
            default:
                LED_A_State = LED_OFF;
                LED_B_State = LED_OFF;
        }
        if (failsafeEngaged) {
            LED_A_State = LED_BLINK_FAST;
            LED_B_State = LED_BLINK_FAST;
        }
        switch (LED_A_State) {
            case LED_OFF:
                LED_A_Clear();
                break;
            case LED_ON:
                LED_A_Set();
                break;
            case LED_BLINK_SLOW:
                if (blinkCount % 10 == 0) {
                    LED_A_Toggle();
                }
                break;
            case LED_BLINK_FAST:
                LED_A_Toggle();
                break;
        }
        switch (LED_B_State) {
            case LED_OFF:
                LED_B_Clear();
                break;
            case LED_ON:
                LED_B_Set();
                break;
            case LED_BLINK_SLOW:
                if (blinkCount % 5 == 0) {
                    LED_B_Toggle();
                }
                break;
            case LED_BLINK_FAST:
                LED_B_Toggle();
                break;
        }
        vTaskDelay(100);
        ++blinkCount;
    }
}
