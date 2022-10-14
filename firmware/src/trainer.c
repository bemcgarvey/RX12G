#include "definitions.h"
#include "autoLevel.h"
#include "gyroTask.h"
#include "attitude.h"
#include "trainer.h"
#include "rxTask.h"

static float rollScale;
static float pitchScale;

void initTrainerMode(void) {
    float maxRange;
    float minRange;
    maxRange = settings.maxTravelLimits[AILERON_INDEX] - channelCenters[aileronChannel];
    minRange = channelCenters[aileronChannel] - settings.minTravelLimits[AILERON_INDEX];
    if (maxRange < minRange) {
        rollScale = settings.rollLimit / maxRange;
    } else {
        rollScale = settings.rollLimit / minRange;
    }
    maxRange = settings.maxTravelLimits[ELEVATOR_INDEX] - channelCenters[elevatorChannel];
    minRange = channelCenters[elevatorChannel] - settings.minTravelLimits[ELEVATOR_INDEX];
    if (maxRange < minRange) {
        pitchScale = settings.pitchLimit / maxRange;
    } else {
        pitchScale = settings.pitchLimit / minRange;
    }
}

void trainerModeCalculate(int axes) {
    float error;
    float deltaError;
    float target;

    if (axes & ROLL_AXIS) {
        rollIgnoreStick = true;
        target = (rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) * rollScale;
        if (target > settings.rollLimit) {
            target = settings.rollLimit;
        } else if (target < -settings.rollLimit) {
            target = -settings.rollLimit;
        }
        error = target - attitude.ypr.roll;
        deltaError = error - lastRollError;
        lastRollError = error;
        rollITerm += error;
        if (rollITerm > settings.rollPID._maxI) {
            rollITerm = settings.rollPID._maxI;
        } else if (rollITerm < -settings.rollPID._maxI) {
            rollITerm = -settings.rollPID._maxI;
        }
        rpyCorrections[ROLL_INDEX] = (error * settings.rollPID._P
                + rollITerm * settings.rollPID._I
                + deltaError * settings.rollPID._D) * rollGains[LEVEL_GAIN];
    }
    if (axes & PITCH_AXIS) {
        pitchIgnoreStick = true;
        target = (rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) * pitchScale;
        if (target > settings.pitchLimit) {
            target = settings.pitchLimit;
        } else if (target < -settings.pitchLimit) {
            target = -settings.pitchLimit;
        }
        error = target - attitude.ypr.pitch;
        deltaError = error - lastPitchError;
        lastPitchError = error;
        pitchITerm += error;
        if (pitchITerm > settings.pitchPID._maxI) {
            pitchITerm = settings.pitchPID._maxI;
        } else if (pitchITerm < -settings.pitchPID._maxI) {
            pitchITerm = -settings.pitchPID._maxI;
        }
        rpyCorrections[PITCH_INDEX] = (error * settings.pitchPID._P
                + pitchITerm * settings.pitchPID._I
                + deltaError * settings.pitchPID._D) * pitchGains[LEVEL_GAIN];
    }
}
