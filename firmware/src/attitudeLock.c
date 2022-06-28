
#include "attitudeLock.h"
#include "attitude.h"
#include "output.h"
#include "rxTask.h"
#include "gyroTask.h"
#include <stdlib.h>

static bool rollLocked;
static float rollSum;
static bool pitchLocked;
static float pitchSum;
static bool yawLocked;
static float yawSum;

#define ERROR_SCALE_FACTOR  0.2

void initAttitudeLock(void) {
    rollLocked = false;
    pitchLocked = false;
    yawLocked = false;
}

void attitudeLockCalculate(int axes) {
    float error;
    float deltaError;
    
    if (axes & ROLL_AXIS) {
        if (!rollLocked) {
            if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) <= deadbands[AILERON_INDEX]) {
                rollLocked = true;
                rollSum = 0;
                lastRollError = 0;
                rollITerm = 0;
            }
        } else if (rollLocked) {
            if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) > deadbands[AILERON_INDEX]) {
                rollLocked = false;
            } else {
                rollSum += attitude.gyroRatesDeg.rollRate;
                error = -rollSum;
                error *= ERROR_SCALE_FACTOR;
                deltaError = error - lastRollError;
                rollITerm += error;
                if (rollITerm > settings.rollPID._maxI) {
                    rollITerm = settings.rollPID._maxI;
                } else if (rollITerm < -settings.rollPID._maxI) {
                    rollITerm = -settings.rollPID._maxI;
                }
                lastRollError = error;
                rpyCorrections[ROLL_INDEX] += error * settings.rollPID._P * rollGain
                        + rollITerm * settings.rollPID._I * rollGain
                        + deltaError * settings.rollPID._D * rollGain;
            }
        }
    }
    if (axes & PITCH_AXIS) {
        if (!pitchLocked) {
            if (abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) <= deadbands[ELEVATOR_INDEX]) {
                pitchLocked = true;
                pitchSum = 0;
                lastPitchError = 0;
                pitchITerm = 0;
            }
        } else if (pitchLocked) {
            if (abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) > deadbands[ELEVATOR_INDEX]) {
                pitchLocked = false;
            } else {
                pitchSum += attitude.gyroRatesDeg.pitchRate;
                error = -pitchSum;
                error *= ERROR_SCALE_FACTOR;
                deltaError = error - lastPitchError;
                pitchITerm += error;
                if (pitchITerm > settings.pitchPID._maxI) {
                    pitchITerm = settings.pitchPID._maxI;
                } else if (pitchITerm < -settings.pitchPID._maxI) {
                    pitchITerm = -settings.pitchPID._maxI;
                }
                lastPitchError = error;
                rpyCorrections[PITCH_INDEX] += error * settings.pitchPID._P * pitchGain
                        + pitchITerm * settings.pitchPID._I * pitchGain
                        + deltaError * settings.pitchPID._D * pitchGain;
            }
        }
    }
    if (axes & YAW_AXIS) {
        if (!yawLocked) {
            if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) <= deadbands[AILERON_INDEX]) {
                yawLocked = true;
                yawSum = 0;
                lastYawError = 0;
                yawITerm = 0;
            }
        } else if (yawLocked) {
            if (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) > deadbands[RUDDER_INDEX]) {
                yawLocked = false;
            } else {
                yawSum += attitude.gyroRatesDeg.yawRate;
                error = -yawSum;
                error *= ERROR_SCALE_FACTOR;
                deltaError = error - lastYawError;
                yawITerm += error;
                if (yawITerm > settings.yawPID._maxI) {
                    yawITerm = settings.yawPID._maxI;
                } else if (yawITerm < -settings.yawPID._maxI) {
                    yawITerm = -settings.yawPID._maxI;
                }
                lastRollError = error;
                rpyCorrections[YAW_INDEX] += error * settings.yawPID._P * yawGain
                        + yawITerm * settings.yawPID._I * yawGain
                        + deltaError * settings.yawPID._D * yawGain;
            }
        }
    }
}