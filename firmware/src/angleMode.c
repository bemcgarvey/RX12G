#include "definitions.h"
#include "gyroTask.h"
#include "attitude.h"
#include "rxTask.h"
#include "settings.h"

static float angleRollScale;
static float anglePitchScale;
static float angleYawScale;

void initAngleMode(void) {
    float maxRange;
    float minRange;
    maxRange = settings.maxTravelLimits[AILERON_INDEX] - channelCenters[aileronChannel];
    minRange = channelCenters[aileronChannel] - settings.minTravelLimits[AILERON_INDEX];
    if (maxRange > minRange) {
        angleRollScale = settings.maxRollRate / maxRange;
    } else {
        angleRollScale = settings.maxRollRate / minRange;
    }
    if (settings.gyroReverseFlags & AILERON_MASK) {
        angleRollScale = -angleRollScale;
    }
    maxRange = settings.maxTravelLimits[ELEVATOR_INDEX] - channelCenters[elevatorChannel];
    minRange = channelCenters[elevatorChannel] - settings.minTravelLimits[ELEVATOR_INDEX];
    if (maxRange > minRange) {
        anglePitchScale = settings.maxPitchRate / maxRange;
    } else {
        anglePitchScale = settings.maxPitchRate / minRange;
    }
    if (settings.gyroReverseFlags & ELEVATOR_MASK) {
        anglePitchScale = -anglePitchScale;
    }
    maxRange = settings.maxTravelLimits[RUDDER_INDEX] - channelCenters[RUDDER];
    minRange = channelCenters[RUDDER] - settings.minTravelLimits[RUDDER_INDEX];
    if (maxRange > minRange) {
        angleYawScale = settings.maxYawRate / maxRange;
    } else {
        angleYawScale = settings.maxYawRate / minRange;
    }
    if (settings.gyroReverseFlags & RUDDER_MASK) {
        angleYawScale = -angleYawScale;
    }
}

void angleModeCalculate(int axes) {
    float error;
    float deltaError;
    float target;
    float gain;
    
    if (axes & ROLL_AXIS) {
        rollIgnoreStick = true;
        target = (rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) * angleRollScale;
        error = target - rateAverages[ROLL_INDEX];
        deltaError = error - lastRollError;
        lastRollError = error;
        rollITerm += error;
        if (rollITerm > settings.rollPID._maxI) {
            rollITerm = settings.rollPID._maxI;
        } else if (rollITerm < -settings.rollPID._maxI) {
            rollITerm = -settings.rollPID._maxI;
        }
        if (rollGains[NORMAL_GAIN] > MIN_GAIN) {
            gain = rollGains[NORMAL_GAIN];
        } else {
            gain = MIN_GAIN;
        }
        rpyCorrections[ROLL_INDEX] = (error * settings.rollPID._P
                + rollITerm * settings.rollPID._I
                + deltaError * settings.rollPID._D) * gain;
    }
    if (axes & PITCH_AXIS) {
        pitchIgnoreStick = true;
        target = (rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) * anglePitchScale;
        error = target - rateAverages[PITCH_INDEX];
        deltaError = error - lastPitchError;
        lastPitchError = error;
        pitchITerm += error;
        if (pitchITerm > settings.pitchPID._maxI) {
            pitchITerm = settings.pitchPID._maxI;
        } else if (pitchITerm < -settings.pitchPID._maxI) {
            pitchITerm = -settings.pitchPID._maxI;
        }
        if (pitchGains[NORMAL_GAIN] > MIN_GAIN) {
            gain = pitchGains[NORMAL_GAIN];
        } else {
            gain = MIN_GAIN;
        }
        rpyCorrections[PITCH_INDEX] = (error * settings.pitchPID._P
                + pitchITerm * settings.pitchPID._I
                + deltaError * settings.pitchPID._D) * gain;
    }
    if (axes & YAW_AXIS) {
        yawIgnoreStick = true;
        target = (rawServoPositions[RUDDER] - channelCenters[RUDDER]) * angleYawScale;
        error = target - rateAverages[YAW_INDEX];
        deltaError = error - lastYawError;
        lastYawError = error;
        yawITerm += error;
        if (yawITerm > settings.yawPID._maxI) {
            yawITerm = settings.yawPID._maxI;
        } else if (yawITerm < -settings.yawPID._maxI) {
            yawITerm = -settings.yawPID._maxI;
        }
        if (yawGains[NORMAL_GAIN] > MIN_GAIN) {
            gain = yawGains[NORMAL_GAIN];
        } else {
            gain = MIN_GAIN;
        }
        rpyCorrections[YAW_INDEX] = (error * settings.yawPID._P
                + yawITerm * settings.yawPID._I
                + deltaError * settings.yawPID._D) * gain;
    }
}
