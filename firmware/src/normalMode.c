
#include "normalMode.h"
#include "attitude.h"
#include "gyroTask.h"
#include "settings.h"
#include "output.h"
#include "rxTask.h"
#include <stdlib.h>

static float dRates[3];
static float lastRates[3];
static int16_t dSticks[3];
static int16_t lastSticks[3];

static float lastRollError;
static float rollITerm;
static float lastPitchError;
static float pitchITerm;
static float lastYawError;
static float yawITerm;

static float stickRanges[3];
#define MAX(a,b)    (a > b ? a : b)
#define ERROR_SCALE     0.4;  //TODO determine the best value for this
#define STICK_MOVE_FACTOR   5  //This determines how fast the gain drops with stick speed

void initNormalMode(void) {
    for (int i = 0; i < 3; ++i) {
        lastRates[i] = 0;
        lastSticks[i] = 0;
        stickRanges[i] = MAX(channelCenters[AILERON + i] - settings.minTravelLimits[i],
                settings.maxTravelLimits[i] - channelCenters[AILERON + i]);
    }
    lastRollError = 0;
    rollITerm = 0;
    lastPitchError = 0;
    pitchITerm = 0;
    lastYawError = 0;
    yawITerm = 0;
}

void normalModeCalculate(int axes) {
    float error;
    float dError;
    float adjust;
    /*dRates[PITCH_INDEX] = attitude.gyroRatesDeg.pitchRate - lastRates[PITCH_INDEX];
    dRates[YAW_INDEX] = attitude.gyroRatesDeg.yawRate - lastRates[YAW_INDEX];
    lastRates[PITCH_INDEX] = attitude.gyroRatesDeg.pitchRate;
    lastRates[YAW_INDEX] = attitude.gyroRatesDeg.yawRate;
    dSticks[PITCH_INDEX] = rawServoPositions[ELEVATOR] - lastSticks[PITCH_INDEX];
    dSticks[YAW_INDEX] = rawServoPositions[RUDDER] - lastSticks[YAW_INDEX];
    lastSticks[PITCH_INDEX] = rawServoPositions[ELEVATOR];
    lastSticks[YAW_INDEX] = rawServoPositions[RUDDER];*/

    if (axes & ROLL_AXIS) {
        dRates[ROLL_INDEX] = rateAverages[ROLL_INDEX] - lastRates[ROLL_INDEX];
        attitude.ypr.yaw = rateAverages[ROLL_INDEX];
        lastRates[ROLL_INDEX] = rateAverages[ROLL_INDEX];
        dSticks[ROLL_INDEX] = rawServoPositions[AILERON] - lastSticks[ROLL_INDEX];
        lastSticks[ROLL_INDEX] = rawServoPositions[AILERON];
        error = -dRates[ROLL_INDEX];
        error = (error + lastRollError) / 2.0;
        //Adjust for stick position
        if (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) < deadbands[ROLL_INDEX]) {
            adjust = 1.0;
        } else {
            switch (settings.gainCurves[AILERON_INDEX]) {
                case GAIN_CURVE_NORMAL:
                    adjust = 1.0 - (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) / stickRanges[AILERON_INDEX]);
                    break;
                case GAIN_CURVE_FLAT:
                    adjust = 1.0 - (0.5 * (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) / stickRanges[AILERON_INDEX]));
                    break;
                case GAIN_CURVE_STEEP:
                    adjust = 1.0 - (2.0 * (abs(rawServoPositions[AILERON] - channelCenters[AILERON]) / stickRanges[AILERON_INDEX]));
                    break;
            }
            if (adjust < 0.0) {
                adjust = 0.0;
            }
        }
        error *= adjust;
        //Adjust for stick motion
        if (abs(dSticks[AILERON_INDEX]) > stickRanges[AILERON_INDEX]) {
            adjust = 0.0;
        } else {
            adjust = (1.0 - (abs(STICK_MOVE_FACTOR * dSticks[AILERON_INDEX]) / stickRanges[AILERON_INDEX]));
            if (adjust < 0.0) {
                adjust = 0.0;
            }
        }
        error *= adjust;
        //Scale to allow for same PIDs as autolevel etc.
        error *= ERROR_SCALE;
        dError = error - lastRollError;
        lastRollError = error;
        rollITerm += error;
        if (rollITerm > settings.rollPID._maxI) {
            rollITerm = settings.rollPID._maxI;
        } else if (rollITerm < -settings.rollPID._maxI) {
            rollITerm = -settings.rollPID._maxI;
        }
        lastRollError = error;
        rpyCorrections[ROLL_INDEX] = (error * settings.rollPID._P
                + rollITerm * settings.rollPID._I
                + dError * settings.rollPID._D) * rollGain;
    }
}
