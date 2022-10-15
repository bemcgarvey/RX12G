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
#include "imu.h"

TaskHandle_t statusLedTaskHandle;

typedef enum {
    LED_OFF = 0, LED_ON, LED_BLINK_SLOW_A, LED_BLINK_FAST_A,
    LED_BLINK_SLOW_B, LED_BLINK_FAST_B
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
        switch (currentFlightMode) {
            case OFF_MODE:
                LED_A_State = LED_OFF;
                LED_B_State = LED_OFF;
                break;
            case NORMAL_MODE:
                LED_A_State = LED_OFF;
                LED_B_State = LED_ON;
                break;
            case AUTO_LEVEL_MODE:
                LED_A_State = LED_ON;
                LED_B_State = LED_OFF;
                break;
            case TRAINER_MODE:
                LED_A_State = LED_BLINK_FAST_A;
                LED_B_State = LED_OFF;
                break;
            case ATTITUDE_LOCK_MODE:
                LED_A_State = LED_ON;
                LED_B_State = LED_ON;
                break;
            case LAUNCH_ASSIST_MODE:
                LED_A_State = LED_OFF;
                LED_B_State = LED_BLINK_FAST_A;
                break;
            case ANGLE_MODE:
                LED_A_State = LED_OFF;
                LED_B_State = LED_BLINK_SLOW_A;
                break;
            case CUSTOM_MODE_1:
                LED_A_State = LED_ON;
                LED_B_State = LED_BLINK_FAST_A;
                break;
            case CUSTOM_MODE_2:
                LED_A_State = LED_BLINK_FAST_A;
                LED_B_State = LED_ON;
                break;
            default:
                LED_A_State = LED_OFF;
                LED_B_State = LED_OFF;
        }
        if (isBinding) {
            LED_A_State = LED_BLINK_FAST_A;
            LED_B_State = LED_BLINK_FAST_B;
        } else if (!imuReady && (settings.rxOnly != RX_ONLY_MODE)) {
            LED_A_State = LED_BLINK_FAST_A;
            LED_B_State = LED_BLINK_FAST_A;
        } else if (failsafeEngaged) {
            LED_A_State = LED_BLINK_SLOW_B;
            LED_B_State = LED_BLINK_SLOW_A;
        }
        switch (LED_A_State) {
            case LED_OFF:
                LED_A_Clear();
                break;
            case LED_ON:
                LED_A_Set();
                break;
            case LED_BLINK_SLOW_A:
                if (blinkCount % 20 == 0) {
                    LED_A_Clear();
                } else if (blinkCount % 10 == 0) {
                    LED_A_Set();
                }
                break;
            case LED_BLINK_SLOW_B:
                if (blinkCount % 20 == 0) {
                    LED_A_Set();
                } else if (blinkCount % 10 == 0) {
                    LED_A_Clear();
                }
                break;
            case LED_BLINK_FAST_A:
                if (blinkCount % 2 == 0) {
                    LED_A_Clear();
                } else {
                    LED_A_Set();
                }
                break;
            case LED_BLINK_FAST_B:
                if (blinkCount % 2 == 0) {
                    LED_A_Set();
                } else {
                    LED_A_Clear();
                }
                break;
        }
        switch (LED_B_State) {
            case LED_OFF:
                LED_B_Clear();
                break;
            case LED_ON:
                LED_B_Set();
                break;
            case LED_BLINK_SLOW_A:
                if (blinkCount % 20 == 0) {
                    LED_B_Clear();
                } else if (blinkCount % 10 == 0) {
                    LED_B_Set();
                }
                break;
            case LED_BLINK_SLOW_B:
                if (blinkCount % 20 == 0) {
                    LED_B_Set();
                } else if (blinkCount % 10 == 0) {
                    LED_B_Clear();
                }
                break;
            case LED_BLINK_FAST_A:
                if (blinkCount % 2 == 0) {
                    LED_B_Clear();
                } else {
                    LED_B_Set();
                }
                break;
            case LED_BLINK_FAST_B:
                if (blinkCount % 2 == 0) {
                    LED_B_Set();
                } else {
                    LED_B_Clear();
                }
                break;
        }
        vTaskDelay(100);
        ++blinkCount;
    }
}
