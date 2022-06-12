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
#include "attitude.h"
#include "autoLevel.h"
#include "rxTask.h"

TaskHandle_t gyroTaskHandle;
FlightModeType currentFlightMode = OFF_MODE;
float rollGain;
float pitchGain;
float yawGain;
volatile uint16_t rawServoPositions[MAX_CHANNELS];
int16_t imuData[6];
bool needToUpdateOutputs;
uint16_t rpyCorrections[3];
int16_t deadbands[3];

static float rollBaseGain;
static float pitchBaseGain;
static float yawBaseGain;
static int modeChannel;
static int rollGainChannel;
static int pitchGainChannel;
static int yawGainChannel;
static int16_t newServoPositions[5];
static bool attitudeInitialized;
static bool doWiggle;
static int wiggleCount;

FlightModeType decodeFlightMode(void);
void calculateGains();
void verifyAndSetOutputs(void);

void gyroTask(void *pvParameters) {
    int16_t temp;
    portTASK_USES_FLOATING_POINT();
    attitudeInitialized = false;
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
    deadbands[ROLL_INDEX] = (settings.deadbands[ROLL_INDEX] * 1024) / 100;
    deadbands[PITCH_INDEX] = (settings.deadbands[PITCH_INDEX] * 1024) / 100;
    deadbands[YAW_INDEX] = (settings.deadbands[YAW_INDEX] * 1024) / 100;
    initAutoLevel();
    needToUpdateOutputs = false;
    doWiggle = false;
    wiggleCount = settings.outputHz;
    while (1) {
        if (xQueueReceive(imuQueue, imuData, 3) == pdTRUE) {
            //Remap axis based on orientation
            switch (settings.gyroOrientation) {
                case FLAT_ORIENTATION:
                    imuData[IMU_ACCEL_X] = -imuData[IMU_ACCEL_X];
                    imuData[IMU_GYRO_Y] = -imuData[IMU_GYRO_Y];
                    imuData[IMU_ACCEL_Y] = -imuData[IMU_ACCEL_Y];
                    imuData[IMU_GYRO_X] = -imuData[IMU_GYRO_X];
                    break;
                case INVERTED_ORIENTATION:
                    imuData[IMU_ACCEL_Z] = -imuData[IMU_ACCEL_Z];
                    imuData[IMU_GYRO_Z] = -imuData[IMU_GYRO_Z];
                    imuData[IMU_ACCEL_X] = -imuData[IMU_ACCEL_X];
                    imuData[IMU_GYRO_X] = -imuData[IMU_GYRO_X];
                    break;
                case LEFT_DOWN_ORIENTATION:
                    imuData[IMU_ACCEL_X] = -imuData[IMU_ACCEL_X];
                    imuData[IMU_GYRO_X] = -imuData[IMU_GYRO_X];
                    temp = imuData[IMU_ACCEL_Y];
                    imuData[IMU_ACCEL_Y] = -imuData[IMU_ACCEL_Z];
                    imuData[IMU_ACCEL_Z] = -temp;
                    temp = imuData[IMU_GYRO_Z];
                    imuData[IMU_GYRO_Z] = -imuData[IMU_GYRO_Y];
                    imuData[IMU_GYRO_Y] = -temp;
                    break;
                case RIGHT_DOWN_ORIENTATION:
                    imuData[IMU_ACCEL_X] = -imuData[IMU_ACCEL_X];
                    imuData[IMU_GYRO_X] = -imuData[IMU_GYRO_X];
                    temp = imuData[IMU_ACCEL_Y];
                    imuData[IMU_ACCEL_Y] = imuData[IMU_ACCEL_Z];
                    imuData[IMU_ACCEL_Z] = temp;
                    temp = imuData[IMU_GYRO_Z];
                    imuData[IMU_GYRO_Z] = imuData[IMU_GYRO_Y];
                    imuData[IMU_GYRO_Y] = temp;
                    break;
            }
            if (!attitudeInitialized) {
                initAttitude();
                attitudeInitialized = true;
                doWiggle = true;
            }
            updateAttitude();
        } else {
            //TODO No imu data available. How do we handle this?  
        }
        if (needToUpdateOutputs) {
            needToUpdateOutputs = false;
            currentFlightMode = decodeFlightMode();
            calculateGains();
            if (doWiggle) {
                if (wiggleCount > 2 * settings.outputHz / 3) {
                    rpyCorrections[AILERON_INDEX] = 200;
                    rpyCorrections[ELEVATOR_INDEX] = 200;
                    rpyCorrections[RUDDER_INDEX] = 0;
                } else if (wiggleCount > settings.outputHz / 3) {
                    rpyCorrections[AILERON_INDEX] = -200;
                    rpyCorrections[ELEVATOR_INDEX] = -200;
                    rpyCorrections[RUDDER_INDEX] = 0;
                } else if (wiggleCount > 0) {
                    rpyCorrections[AILERON_INDEX] = 0;
                    rpyCorrections[ELEVATOR_INDEX] = 0;
                    rpyCorrections[RUDDER_INDEX] = 0;
                } else {
                    doWiggle = false;
                }
                if (wiggleCount != 0) {
                    --wiggleCount;
                }
                verifyAndSetOutputs();                
            } else if (currentFlightMode == OFF_MODE || !attitudeInitialized) {
                for (int i = 0; i < MAX_CHANNELS; ++i) {
                    outputServos[i] = rawServoPositions[i];
                }
            } else if (currentFlightMode == NORMAL_MODE) {

            } else if (currentFlightMode == AUTO_LEVEL_MODE) {
                autoLevelCalculate(ROLL_AXIS | PITCH_AXIS | YAW_AXIS);
                verifyAndSetOutputs();
            } else if (currentFlightMode == ATTITUDE_LOCK_MODE) {

            } else if (currentFlightMode == LAUNCH_ASSIST_MODE) {
                autoLevelCalculate(ROLL_AXIS);
                //launchAssistCalculate(PITCH_AXIS);
                verifyAndSetOutputs();
            } else {
                //Mode is unrecognized so same as off
                for (int i = 0; i < MAX_CHANNELS; ++i) {
                    outputServos[i] = rawServoPositions[i];
                }
            }
        }
        //TODO check stack level - remove when done
        //int stack = uxTaskGetStackHighWaterMark(NULL);
        //if (stack < 1) {
        //    SAT2_LED_Set();
        //    while (1);
        //}
    }
}

FlightModeType decodeFlightMode(void) {
    if (modeChannel == 0) {
        return settings.flightModes[0];
    } else {
        uint16_t value = rawServoPositions[modeChannel];
        if (value == 0xffff) {
            return OFF_MODE; //TODO what should the failsafe flight mode be? Off or auto level?
        }
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

void verifyAndSetOutputs(void) {
    if (settings.gyroEnabledFlags & AILERON_MASK) {
        if (settings.gyroReverseFlags & AILERON_MASK) {
            newServoPositions[AILERON_INDEX] = rawServoPositions[AILERON] - rpyCorrections[AILERON_INDEX];
        } else {
            newServoPositions[AILERON_INDEX] = rawServoPositions[AILERON] + rpyCorrections[AILERON_INDEX];
        }
    } else {
        newServoPositions[AILERON_INDEX] = rawServoPositions[AILERON];
    }
    if (newServoPositions[AILERON_INDEX] < settings.minTravelLimits[AILERON_INDEX]) {
        newServoPositions[AILERON_INDEX] = settings.minTravelLimits[AILERON_INDEX];
    } else if (newServoPositions[AILERON_INDEX] > settings.maxTravelLimits[AILERON_INDEX]) {
        newServoPositions[AILERON_INDEX] = settings.maxTravelLimits[AILERON_INDEX];
    }
    if (settings.gyroEnabledFlags & ELEVATOR_MASK) {
        if (settings.gyroReverseFlags & ELEVATOR_MASK) {
            newServoPositions[ELEVATOR_INDEX] = rawServoPositions[ELEVATOR] - rpyCorrections[ELEVATOR_INDEX];
        } else {
            newServoPositions[ELEVATOR_INDEX] = rawServoPositions[ELEVATOR] + rpyCorrections[ELEVATOR_INDEX];
        }
    } else {
        newServoPositions[ELEVATOR_INDEX] = rawServoPositions[ELEVATOR];
    }
    if (newServoPositions[ELEVATOR_INDEX] < settings.minTravelLimits[ELEVATOR_INDEX]) {
        newServoPositions[ELEVATOR_INDEX] = settings.minTravelLimits[ELEVATOR_INDEX];
    } else if (newServoPositions[ELEVATOR_INDEX] > settings.maxTravelLimits[ELEVATOR_INDEX]) {
        newServoPositions[ELEVATOR_INDEX] = settings.maxTravelLimits[ELEVATOR_INDEX];
    }
    if (settings.gyroEnabledFlags & RUDDER_MASK) {
        if (settings.gyroReverseFlags & RUDDER_MASK) {
            newServoPositions[RUDDER_INDEX] = rawServoPositions[RUDDER] - rpyCorrections[RUDDER_INDEX];
        } else {
            newServoPositions[RUDDER_INDEX] = rawServoPositions[RUDDER] + rpyCorrections[RUDDER_INDEX];
        }
    } else {
        newServoPositions[RUDDER_INDEX] = rawServoPositions[RUDDER];
    }
    if (newServoPositions[RUDDER_INDEX] < settings.minTravelLimits[RUDDER_INDEX]) {
        newServoPositions[RUDDER_INDEX] = settings.minTravelLimits[RUDDER_INDEX];
    } else if (newServoPositions[RUDDER_INDEX] > settings.maxTravelLimits[RUDDER_INDEX]) {
        newServoPositions[RUDDER_INDEX] = settings.maxTravelLimits[RUDDER_INDEX];
    }
    if (settings.gyroEnabledFlags & AILERON2_MASK) {
        if (settings.gyroReverseFlags & AILERON2_MASK) {
            newServoPositions[AILERON2_INDEX] = rawServoPositions[settings.aileron2Channel] + rpyCorrections[AILERON_INDEX];
        } else {
            newServoPositions[AILERON2_INDEX] = rawServoPositions[settings.aileron2Channel] - rpyCorrections[AILERON_INDEX];
        }
    } else {
        newServoPositions[AILERON2_INDEX] = rawServoPositions[settings.aileron2Channel];
    }
    if (newServoPositions[AILERON2_INDEX] < settings.minTravelLimits[AILERON2_INDEX]) {
        newServoPositions[AILERON2_INDEX] = settings.minTravelLimits[AILERON2_INDEX];
    } else if (newServoPositions[AILERON2_INDEX] > settings.maxTravelLimits[AILERON2_INDEX]) {
        newServoPositions[AILERON2_INDEX] = settings.maxTravelLimits[AILERON2_INDEX];
    }
    if (settings.gyroEnabledFlags & ELEVATOR2_MASK) {
        if (settings.gyroReverseFlags & ELEVATOR2_MASK) {
            newServoPositions[ELEVATOR2_INDEX] = rawServoPositions[settings.elevator2Channel] - rpyCorrections[ELEVATOR2_INDEX];
        } else {
            newServoPositions[ELEVATOR2_INDEX] = rawServoPositions[settings.elevator2Channel] + rpyCorrections[ELEVATOR2_INDEX];
        }
    } else {
        newServoPositions[ELEVATOR2_INDEX] = rawServoPositions[settings.elevator2Channel];
    }
    if (newServoPositions[ELEVATOR2_INDEX] < settings.minTravelLimits[ELEVATOR2_INDEX]) {
        newServoPositions[ELEVATOR2_INDEX] = settings.minTravelLimits[ELEVATOR2_INDEX];
    } else if (newServoPositions[ELEVATOR2_INDEX] > settings.maxTravelLimits[ELEVATOR2_INDEX]) {
        newServoPositions[ELEVATOR2_INDEX] = settings.maxTravelLimits[ELEVATOR2_INDEX];
    }
    outputServos[THROTTLE] = rawServoPositions[THROTTLE];
    outputServos[AILERON] = newServoPositions[AILERON_INDEX];
    outputServos[ELEVATOR] = newServoPositions[ELEVATOR_INDEX];
    outputServos[RUDDER] = newServoPositions[RUDDER_INDEX];
    for (int i = GEAR; i < MAX_CHANNELS; ++i) {
        if (i == settings.aileron2Channel) {
            outputServos[i] = newServoPositions[AILERON2_INDEX];
        } else if (i == settings.elevator2Channel) {
            outputServos[i] = newServoPositions[ELEVATOR2_INDEX];
        } else {
            outputServos[i] = rawServoPositions[i];
        }
    }
}

bool sticksCentered(void) {
    static int centerCount = CENTER_COUNT;
    if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) < deadbands[ROLL_INDEX]
            && abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) < deadbands[PITCH_INDEX]
            && abs(rawServoPositions[AILERON] - channelCenters[AILERON]) < deadbands[ROLL_INDEX]) {
        if (centerCount > 0) {
            --centerCount;
        } else {
            return true;
        }
    } else {
        centerCount = CENTER_COUNT;
    }
    return false;
}