
#include "attitudeLock.h"
#include "attitude.h"
#include "output.h"
#include "rxTask.h"
#include "gyroTask.h"
#include <stdlib.h>

static bool rollLocked;
static float rollTarget;
static bool pitchLocked;
static float pitchTarget;
static bool yawLocked;
static float yawTarget;

static float lastRollError;
static float rollITerm;
static float lastPitchError;
static float pitchITerm;
static float lastYawError;
static float yawITerm;

void initAttitudeLock(void) {
    rollLocked = false;
    pitchLocked = false;
    yawLocked = false;
}

void attitudeLockCalculate(int axes) {
    float error;
    float deltaError;

    //TODO pitch and yaw are in the world frame so the surface to adjust depends on the roll
    //How do we put these in the aircraft frame?
    //Also when inverted pitch is reversed.
    
    if (axes & ROLL_AXIS) {
        if (!rollLocked) {
            if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) <= deadbands[AILERON_INDEX]) {
                rollLocked = true;
                rollTarget = attitude.ypr.roll;
                lastRollError = 0;
                rollITerm = 0;
            }
        } else if (rollLocked) {
            if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) > deadbands[AILERON_INDEX]) {
                rollLocked = false;
            } else {
                error = rollTarget - attitude.ypr.roll;
                deltaError = error - lastRollError;
                rollITerm += error;
                if (rollITerm > settings.rollPID._maxI) {
                    rollITerm = settings.rollPID._maxI;
                } else if (rollITerm < -settings.rollPID._maxI) {
                    rollITerm = -settings.rollPID._maxI;
                }
                lastRollError = error;
                rpyCorrections[ROLL_INDEX] = error * settings.rollPID._P * rollGain
                        + rollITerm * settings.rollPID._I * rollGain
                        + deltaError * settings.rollPID._D * rollGain;
            }
        }
    }
    if (axes & PITCH_AXIS) {
        if (!pitchLocked) {
            if (abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) <= deadbands[ELEVATOR_INDEX]) {
                pitchLocked = true;
                pitchTarget = attitude.ypr.pitch;
                lastPitchError = 0;
                pitchITerm = 0;
            }
        } else if (pitchLocked) {
            if (abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) > deadbands[ELEVATOR_INDEX]) {
                pitchLocked = false;
            } else {
                error = pitchTarget - attitude.ypr.pitch;
                deltaError = error - lastPitchError;
                pitchITerm += error;
                if (pitchITerm > settings.pitchPID._maxI) {
                    pitchITerm = settings.pitchPID._maxI;
                } else if (pitchITerm < -settings.pitchPID._maxI) {
                    pitchITerm = -settings.pitchPID._maxI;
                }
                lastPitchError = error;
                rpyCorrections[PITCH_INDEX] = error * settings.pitchPID._P * pitchGain
                        + pitchITerm * settings.pitchPID._I * pitchGain
                        + deltaError * settings.pitchPID._D * pitchGain;
            }
        }
    }
    if (axes & YAW_AXIS) {
        if (!yawLocked) {
            if (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) <= deadbands[RUDDER_INDEX]) {
                yawLocked = true;
                yawTarget = attitude.ypr.yaw;
                lastYawError = 0;
                yawITerm = 0;
            }
        } else if (yawLocked) {
            if (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) > deadbands[RUDDER_INDEX]) {
                yawLocked = false;
            } else {
                error = yawTarget - attitude.ypr.yaw;
                deltaError = error - lastYawError;
                yawITerm += error;
                if (yawITerm > settings.yawPID._maxI) {
                    yawITerm = settings.yawPID._maxI;
                } else if (yawITerm < -settings.yawPID._maxI) {
                    yawITerm = -settings.yawPID._maxI;
                }
                lastRollError = error;
                rpyCorrections[YAW_INDEX] = error * settings.yawPID._P * yawGain
                        + yawITerm * settings.yawPID._I * yawGain
                        + deltaError * settings.yawPID._D * yawGain;
            }
        }
    }
}