/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: attitudeLock.c                            //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: attitude lock mode                 //
/////////////////////////////////////////////////////

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
            if (abs(rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) <= deadbands[AILERON_INDEX]) {
                rollLocked = true;
                rollSum = 0;
                lastRollError = 0;
                rollITerm = 0;
            }
        }
        if (rollLocked) {
            if (abs(rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) > deadbands[AILERON_INDEX]) {
                rollLocked = false;
            } else {
                rollSum += (rateAverages[ROLL_INDEX] * (CONTROL_LOOP_PERIOD / 1000.0));
                error = -rollSum;
                deltaError = error - lastRollError;
                rollITerm += error;
                if (rollITerm > settings.rollPID._maxI) {
                    rollITerm = settings.rollPID._maxI;
                } else if (rollITerm < -settings.rollPID._maxI) {
                    rollITerm = -settings.rollPID._maxI;
                }
                lastRollError = error;
                rpyCorrections[ROLL_INDEX] += (error * settings.rollPID._P
                        + rollITerm * settings.rollPID._I
                        + deltaError * settings.rollPID._D) * rollGains[LOCK_GAIN];
            }
        }
    }
    if (axes & PITCH_AXIS) {
        if (!pitchLocked) {
            if (abs(rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) <= deadbands[ELEVATOR_INDEX]) {
                pitchLocked = true;
                pitchSum = 0;
                lastPitchError = 0;
                pitchITerm = 0;
            }
        }
        if (pitchLocked) {
            if (abs(rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) > deadbands[ELEVATOR_INDEX]) {
                pitchLocked = false;
            } else {
                pitchSum += (attitude.gyroRatesDeg.pitchRate * (CONTROL_LOOP_PERIOD / 1000.0));
                error = -pitchSum;
                deltaError = error - lastPitchError;
                pitchITerm += error;
                if (pitchITerm > settings.pitchPID._maxI) {
                    pitchITerm = settings.pitchPID._maxI;
                } else if (pitchITerm < -settings.pitchPID._maxI) {
                    pitchITerm = -settings.pitchPID._maxI;
                }
                lastPitchError = error;
                rpyCorrections[PITCH_INDEX] += (error * settings.pitchPID._P
                        + pitchITerm * settings.pitchPID._I
                        + deltaError * settings.pitchPID._D) * pitchGains[LOCK_GAIN];
            }
        }
    }
    if (axes & YAW_AXIS) {
        if (!yawLocked) {
            if (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) <= deadbands[RUDDER_INDEX]) {
                yawLocked = true;
                yawSum = 0;
                lastYawError = 0;
                yawITerm = 0;
            }
        }
        if (yawLocked) {
            if (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) > deadbands[RUDDER_INDEX]) {
                yawLocked = false;
            } else {
                yawSum += (attitude.gyroRatesDeg.yawRate * (CONTROL_LOOP_PERIOD / 1000.0));
                error = -yawSum;
                deltaError = error - lastYawError;
                yawITerm += error;
                if (yawITerm > settings.yawPID._maxI) {
                    yawITerm = settings.yawPID._maxI;
                } else if (yawITerm < -settings.yawPID._maxI) {
                    yawITerm = -settings.yawPID._maxI;
                }
                lastYawError = error;
                rpyCorrections[YAW_INDEX] += (error * settings.yawPID._P
                        + yawITerm * settings.yawPID._I
                        + deltaError * settings.yawPID._D) * yawGains[LOCK_GAIN];
            }
        }
    }
}