/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: gyroTask.c                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: gyro and stability code            //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "gyroTask.h"
#include "rtosHandles.h"
#include "output.h"
#include "tasks.h"

FlightModeType currentFlightMode = OFF_MODE;
TaskHandle_t gyroTaskHandle;

volatile uint16_t rawServoPositions[MAX_CHANNELS];

static uint16_t aileron2Value;
static uint16_t elevator2Value;
static float rollBaseGain;
static float pitchBaseGain;
static float yawBaseGain;
static float rollGain;
static float pitchGain;
static float yawGain;
int modeChannel;
int rollGainChannel;
int pitchGainChannel;
int yawGainChannel;

FlightModeType decodeFlightMode(void);
void calculateGains();

void gyroTask(void *pvParameters) {
    portTASK_USES_FLOATING_POINT();
    aileron2Value = 0xffff;
    elevator2Value = 0xffff;
    modeChannel = settings.flightModeChannel;
    if (settings.numFlightModes == 1) {
        modeChannel = 0;
    }
    rollGainChannel = settings.gainChannels[ROLL_INDEX];
    pitchGainChannel = settings.gainChannels[PITCH_INDEX];
    yawGainChannel = settings.gainChannels[YAW_INDEX];
    rollBaseGain = settings.gains[ROLL_INDEX] / 100.0;
    pitchBaseGain = settings.gains[PITCH_INDEX] / 100.0;
    yawBaseGain = settings.gains[YAW_INDEX] / 100.0;
    while (1) {
        currentFlightMode = decodeFlightMode();
        calculateGains();
        if (currentFlightMode == OFF_MODE || startMode == START_USB) {
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        } else if (currentFlightMode == NORMAL_MODE) {

        } else if (currentFlightMode == AUTO_LEVEL_MODE) {

        } else if (currentFlightMode == ATTITUDE_LOCK_MODE) {

        } else if (currentFlightMode == LAUNCH_ASSIST_MODE) {

        } else {
            //Mode is unrecognized so same as off
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        }
        vTaskDelay(5); //TODO should this be replaced by a notification from a timer?
        // Does it need to be that accurate?
        //TODO check stack level - remove when done
        int stack = uxTaskGetStackHighWaterMark(NULL);
        if (stack < 1) {
            SAT2_LED_Set();
            while (1);
        }
    }
}

FlightModeType decodeFlightMode(void) {
    if (modeChannel == 0) {
        return settings.flightModes[0];
    } else {
        uint16_t value = rawServoPositions[modeChannel];
        if (settings.numFlightModes == 3) {
            if (value < 682) {
                return settings.flightModes[0];
            }
            if (value < 1364) {
                return settings.flightModes[1];
            }
            return settings.flightModes[2];
        } else { //6 modes
            if (value < 341) {
                return settings.flightModes[0];
            }
            if (value < 682) {
                return settings.flightModes[1];
            }
            if (value < 1024) {
                return settings.flightModes[2];
            }
            if (value < 1364) {
                return settings.flightModes[3];
            }
            if (value < 1705) {
                return settings.flightModes[4];
            }
            return settings.flightModes[5];
        }
    }
}

void calculateGains() {
    //TODO factor in gain curve
    if (rollGainChannel) {
        rollGain = rollBaseGain * rawServoPositions[rollGainChannel] / 2047.0;
    } else {
        rollGain = rollBaseGain;
    }
    if (pitchGainChannel) {
        pitchGain = pitchBaseGain * rawServoPositions[pitchGainChannel] / 2047.0;
    } else {
        pitchGain = pitchBaseGain;
    }
    if (yawGainChannel) {
        yawGain = yawBaseGain * rawServoPositions[yawGainChannel] / 2047.0;
    } else {
        yawGain = yawBaseGain;
    }
}