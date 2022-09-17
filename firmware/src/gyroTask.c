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
#include "imu.h"
#include "launchAssist.h"
#include "normalMode.h"
#include "attitudeLock.h"
#include "trainer.h"

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
int centerCount;
float rateAverages[3];
int avgCount;

//PID variables used by all modes
float lastRollError;
float rollITerm;
float lastPitchError;
float pitchITerm;
float lastYawError;
float yawITerm;

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
static int imuMissedCount;


FlightModeType decodeFlightMode(void);
void calculateGains(void);
void verifyAndSetOutputs(void);
void initPIDValues(void);

void gyroTask(void *pvParameters) {
    int16_t temp;
    FlightModeType newMode;
    portTASK_USES_FLOATING_POINT();
    attitudeInitialized = false;
    currentFlightMode = OFF_MODE;
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
    needToUpdateOutputs = false;
    doWiggle = false;
    wiggleCount = CONTROL_LOOP_FREQ / 2;
    imuMissedCount = 0;
    centerCount = CENTER_COUNT;
    for (int i = 0; i < 3; ++i) {
        rateAverages[i] = 0;
    }
    avgCount = 0;
    WDT_Enable();
    while (1) {
        if (xQueueReceive(imuQueue, imuData, 3) == pdTRUE) {
            imuMissedCount = 0;
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
                if (startMode != START_WDTO) {
                    doWiggle = true;
                }
            }
            updateAttitude();
            rateAverages[ROLL_INDEX] += attitude.gyroRatesDeg.rollRate;
            rateAverages[PITCH_INDEX] += attitude.gyroRatesDeg.pitchRate;
            rateAverages[YAW_INDEX] += attitude.gyroRatesDeg.yawRate;
            ++avgCount;
        } else {
            if (attitudeInitialized) {
                ++imuMissedCount;
                if (imuMissedCount >= GYRO_ODR) {
                    attitudeInitialized = false;
                    vTaskSuspend(imuTaskHandle);
                }
            }
        }
        if (needToUpdateOutputs) {
            needToUpdateOutputs = false;
            rpyCorrections[AILERON_INDEX] = 0;
            rpyCorrections[ELEVATOR_INDEX] = 0;
            rpyCorrections[RUDDER_INDEX] = 0;
            for (int i = 0; i < 3; ++i) {
                rateAverages[i] /= (float) avgCount;
            }
            newMode = decodeFlightMode();
            if (attitudeInitialized && newMode != currentFlightMode) {
                currentFlightMode = newMode;
                initPIDValues();
                switch (currentFlightMode) {
                    case NORMAL_MODE:
                        initNormalMode();
                        break;
                    case AUTO_LEVEL_MODE:
                        initAutoLevel();
                        initNormalMode();
                        break;
                    case LAUNCH_ASSIST_MODE:
                        initLaunchAssist();
                        initAutoLevel();
                        initNormalMode();
                        break;
                    case ATTITUDE_LOCK_MODE:
                        initAttitudeLock();
                        break;
                    case TRAINER_MODE:
                        initTrainerMode();
                        initAutoLevel();
                        initNormalMode();
                        break;
                    case CUSTOM_MODE_1:
                    case CUSTOM_MODE_2:
                        initLaunchAssist();
                        initAutoLevel();
                        initNormalMode();
                        initTrainerMode();
                        initAttitudeLock();
                        break;
                    default:
                        break;
                }
            }
            calculateGains();
            if (doWiggle) {
                rpyCorrections[AILERON_INDEX] = 0;
                rpyCorrections[ELEVATOR_INDEX] = 0;
                rpyCorrections[RUDDER_INDEX] = 0;
                if (wiggleCount > 2 * settings.outputHz / 6) {
                    if (settings.gyroEnabledFlags & AILERON_MASK) {
                        rpyCorrections[AILERON_INDEX] = 300;
                    }
                    if (settings.gyroEnabledFlags & ELEVATOR_MASK) {
                        rpyCorrections[ELEVATOR_INDEX] = 300;
                    }
                    if (settings.gyroEnabledFlags & RUDDER_MASK) {
                        rpyCorrections[RUDDER_INDEX] = 300;
                    }
                } else if (wiggleCount > settings.outputHz / 6) {
                    if (settings.gyroEnabledFlags & AILERON_MASK) {
                        rpyCorrections[AILERON_INDEX] = -300;
                    }
                    if (settings.gyroEnabledFlags & ELEVATOR_MASK) {
                        rpyCorrections[ELEVATOR_INDEX] = -300;
                    }
                    if (settings.gyroEnabledFlags & RUDDER_MASK) {
                        rpyCorrections[RUDDER_INDEX] = -300;
                    }
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
            } else { //We have an active flight mode
                if (currentFlightMode == NORMAL_MODE) {
                    normalModeCalculate(ROLL_AXIS | PITCH_AXIS | YAW_AXIS);
                } else if (currentFlightMode == AUTO_LEVEL_MODE) {
                    autoLevelCalculate(ROLL_AXIS | PITCH_AXIS);
                    normalModeCalculate(YAW_AXIS);
                } else if (currentFlightMode == ATTITUDE_LOCK_MODE) {
                    attitudeLockCalculate(ROLL_AXIS | PITCH_AXIS | YAW_AXIS);
                } else if (currentFlightMode == LAUNCH_ASSIST_MODE) {
                    autoLevelCalculate(ROLL_AXIS);
                    launchAssistCalculate(PITCH_AXIS);
                    normalModeCalculate(YAW_AXIS);
                } else if (currentFlightMode == TRAINER_MODE) {
                    trainerModeCalculate(ROLL_AXIS | PITCH_AXIS);
                    normalModeCalculate(YAW_AXIS);
                } else if (currentFlightMode == CUSTOM_MODE_1 || currentFlightMode == CUSTOM_MODE_2) {
                    CustomModeType *mode;
                    if (currentFlightMode == CUSTOM_MODE_1) {
                        mode = &settings.customMode1;
                    } else if (currentFlightMode == CUSTOM_MODE_2) {
                        mode = &settings.customMode2;
                    }
                    switch (mode->aileronMode) {
                        case NORMAL_MODE:
                            normalModeCalculate(ROLL_AXIS);
                            break;
                        case AUTO_LEVEL_MODE:
                        case LAUNCH_ASSIST_MODE:
                            autoLevelCalculate(ROLL_AXIS);
                            break;
                        case ATTITUDE_LOCK_MODE:
                            attitudeLockCalculate(ROLL_AXIS);
                            break;
                        case TRAINER_MODE:
                            trainerModeCalculate(ROLL_AXIS);
                            break;
                        case OFF_MODE:
                            rpyCorrections[AILERON_INDEX] = 0;
                            break;
                    }
                    switch (mode->elevatorMode) {
                        case NORMAL_MODE:
                            normalModeCalculate(PITCH_AXIS);
                            break;
                        case AUTO_LEVEL_MODE:
                            autoLevelCalculate(PITCH_AXIS);
                            break;
                        case LAUNCH_ASSIST_MODE:
                            launchAssistCalculate(PITCH_AXIS);
                            break;
                        case ATTITUDE_LOCK_MODE:
                            attitudeLockCalculate(PITCH_AXIS);
                            break;
                        case TRAINER_MODE:
                            trainerModeCalculate(PITCH_AXIS);
                            break;
                        case OFF_MODE:
                            rpyCorrections[ELEVATOR_INDEX] = 0;
                            break;
                    }
                    switch (mode->rudderMode) {
                        case NORMAL_MODE:
                        case AUTO_LEVEL_MODE:
                        case LAUNCH_ASSIST_MODE:
                        case TRAINER_MODE:
                            normalModeCalculate(YAW_AXIS);
                            break;
                        case ATTITUDE_LOCK_MODE:
                            attitudeLockCalculate(YAW_AXIS);
                            break;
                        case OFF_MODE:
                            rpyCorrections[RUDDER_INDEX] = 0;
                            break;
                    }
                }
                verifyAndSetOutputs();
            }
            for (int i = 0; i < 3; ++i) {
                rateAverages[i] = 0.0;
            }
            avgCount = 0;
            WDT_Clear();
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
    if (modeChannel == 0 || settings.numFlightModes == 1) {
        return settings.flightModes[0];
    } else {
        uint16_t value = rawServoPositions[modeChannel];
        if (value == 0xffff) {
            return OFF_MODE;
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

void calculateGains(void) {
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
    if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) < deadbands[ROLL_INDEX]
            && abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) < deadbands[PITCH_INDEX]) {
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

void initPIDValues(void) {
    lastRollError = 0;
    rollITerm = 0;
    lastPitchError = 0;
    pitchITerm = 0;
    lastYawError = 0;
    yawITerm = 0;
}