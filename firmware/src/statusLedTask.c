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
#include "buttonTask.h"
#include "satellites.h"
#include "uart.h"

TaskHandle_t statusLedTaskHandle;

typedef enum {
    LED_OFF = 0, LED_ON, LED_BLINK_SLOW, LED_BLINK_FAST
} LedState;

void statusLedTask(void *pvParameters) {
    bool activity;
    LedState LED_A_State = LED_OFF;
    LedState LED_B_State = LED_OFF;
    int blinkCount = 0;
    while (1) {
        //Check sat activity and set LEDs and failsafe
        activity = false;
        uint32_t time = getSystemTime();
        if (time - lastRxTime[SAT1] < 100) {
            SAT1_LED_Set();
            activity = true;
            failsafeEngaged = false;
        } else {
            SAT1_LED_Clear();
        }
        if (time - lastRxTime[SAT2] < 100) {
            SAT2_LED_Set();
            activity = true;
            failsafeEngaged = false;
        } else {
            SAT2_LED_Clear();
        }
        if (time - lastRxTime[SAT3] < 100) {
            SAT3_LED_Set();
            activity = true;
            failsafeEngaged = false;
        } else {
            SAT3_LED_Clear();
        }
        if (!activity) {
            if (!failsafeEngaged) {
                engageFailsafe();
            }
        }
        
        //Set status LEDs
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
        if (isBinding) {
            LED_A_State = LED_BLINK_FAST;
            LED_B_State = LED_BLINK_FAST;
        }
        else if (failsafeEngaged) {
            LED_A_State = LED_BLINK_SLOW;
            LED_B_State = LED_BLINK_SLOW;
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
                if (blinkCount % 10 == 0) {
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
