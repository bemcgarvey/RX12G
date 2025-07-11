/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: normalMode.c                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: normal rate mode                   //
/////////////////////////////////////////////////////

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

static float stickRanges[3];
#define MAX(a,b)    (a > b ? a : b)
#define STICK_MOVE_FACTOR   5  //This determines how fast the gain drops with stick speed

void initNormalMode(void) {
    for (int i = 0; i < 3; ++i) {
        lastRates[i] = 0;
        lastSticks[i] = 0;
    }
    stickRanges[AILERON_INDEX] = MAX(channelCenters[aileronChannel] - settings.minTravelLimits[AILERON_INDEX],
                settings.maxTravelLimits[AILERON_INDEX] - channelCenters[aileronChannel]);
    stickRanges[ELEVATOR_INDEX] = MAX(channelCenters[elevatorChannel] - settings.minTravelLimits[ELEVATOR_INDEX],
                settings.maxTravelLimits[ELEVATOR_INDEX] - channelCenters[elevatorChannel]);
    stickRanges[RUDDER_INDEX] = MAX(channelCenters[RUDDER] - settings.minTravelLimits[RUDDER_INDEX],
                settings.maxTravelLimits[RUDDER_INDEX] - channelCenters[RUDDER]);
}

void normalModeCalculate(int axes) {
    float error;
    float dError;
    float adjust;

    if (axes & ROLL_AXIS) {
        dRates[ROLL_INDEX] = rateAverages[ROLL_INDEX] - lastRates[ROLL_INDEX];
        lastRates[ROLL_INDEX] = rateAverages[ROLL_INDEX];
        dSticks[ROLL_INDEX] = rawServoPositions[aileronChannel] - lastSticks[ROLL_INDEX];
        lastSticks[ROLL_INDEX] = rawServoPositions[aileronChannel];
        error = -dRates[ROLL_INDEX];
        //Adjust for stick position
        if (abs(rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) < deadbands[ROLL_INDEX]) {
            adjust = 1.0;
        } else {
            switch (settings.gainCurves[AILERON_INDEX]) {
                case GAIN_CURVE_NORMAL:
                    adjust = 1.0 - (abs(rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) / stickRanges[AILERON_INDEX]);
                    break;
                case GAIN_CURVE_FLAT:
                    adjust = 1.0 - (0.5 * (abs(rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) / stickRanges[AILERON_INDEX]));
                    break;
                case GAIN_CURVE_STEEP:
                    adjust = 1.0 - (2.0 * (abs(rawServoPositions[aileronChannel] - channelCenters[aileronChannel]) / stickRanges[AILERON_INDEX]));
                    break;
            }
            if (adjust < 0.0) {
                adjust = 0.0;
            }
        }
        error *= adjust;
        //Adjust for stick motion
        adjust = (1.0 - (abs(STICK_MOVE_FACTOR * dSticks[AILERON_INDEX]) / stickRanges[AILERON_INDEX]));
        if (adjust < 0.0) {
            adjust = 0.0;
        }
        error *= adjust;
        error = (error + 2.0 * lastRollError) / 3.0;
        dError = error - lastRollError;
        lastRollError = error;
        rollITerm += error;
        if (rollITerm > settings.rollPID._maxI) {
            rollITerm = settings.rollPID._maxI;
        } else if (rollITerm < -settings.rollPID._maxI) {
            rollITerm = -settings.rollPID._maxI;
        }
        rpyCorrections[ROLL_INDEX] += (error * settings.rollPID._P
                + rollITerm * settings.rollPID._I
                + dError * settings.rollPID._D) * rollGains[NORMAL_GAIN];
    }
    if (axes & PITCH_AXIS) {
        dRates[PITCH_INDEX] = rateAverages[PITCH_INDEX] - lastRates[PITCH_INDEX];
        lastRates[PITCH_INDEX] = rateAverages[PITCH_INDEX];
        dSticks[PITCH_INDEX] = rawServoPositions[elevatorChannel] - lastSticks[PITCH_INDEX];
        lastSticks[PITCH_INDEX] = rawServoPositions[elevatorChannel];
        error = -dRates[PITCH_INDEX];
        //Adjust for stick position
        if (abs(rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) < deadbands[PITCH_INDEX]) {
            adjust = 1.0;
        } else {
            switch (settings.gainCurves[ELEVATOR_INDEX]) {
                case GAIN_CURVE_NORMAL:
                    adjust = 1.0 - (abs(rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) / stickRanges[ELEVATOR_INDEX]);
                    break;
                case GAIN_CURVE_FLAT:
                    adjust = 1.0 - (0.5 * (abs(rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) / stickRanges[ELEVATOR_INDEX]));
                    break;
                case GAIN_CURVE_STEEP:
                    adjust = 1.0 - (2.0 * (abs(rawServoPositions[elevatorChannel] - channelCenters[elevatorChannel]) / stickRanges[ELEVATOR_INDEX]));
                    break;
            }
            if (adjust < 0.0) {
                adjust = 0.0;
            }
        }
        error *= adjust;
        //Adjust for stick motion
        adjust = (1.0 - (abs(STICK_MOVE_FACTOR * dSticks[ELEVATOR_INDEX]) / stickRanges[ELEVATOR_INDEX]));
        if (adjust < 0.0) {
            adjust = 0.0;
        }
        error *= adjust;
        error = (error + 2.0 * lastPitchError) / 3.0;
        dError = error - lastPitchError;
        lastPitchError = error;
        pitchITerm += error;
        if (pitchITerm > settings.pitchPID._maxI) {
            pitchITerm = settings.pitchPID._maxI;
        } else if (pitchITerm < -settings.pitchPID._maxI) {
            pitchITerm = -settings.pitchPID._maxI;
        }
        lastPitchError = error;
        rpyCorrections[PITCH_INDEX] += (error * settings.pitchPID._P
                + pitchITerm * settings.pitchPID._I
                + dError * settings.pitchPID._D) * pitchGains[NORMAL_GAIN];
    }
    if (axes & YAW_AXIS) {
        dRates[YAW_INDEX] = rateAverages[YAW_INDEX] - lastRates[YAW_INDEX];
        lastRates[YAW_INDEX] = rateAverages[YAW_INDEX];
        dSticks[YAW_INDEX] = rawServoPositions[RUDDER] - lastSticks[YAW_INDEX];
        lastSticks[YAW_INDEX] = rawServoPositions[RUDDER];
        error = -dRates[YAW_INDEX];
        //Adjust for stick position
        if (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) < deadbands[YAW_INDEX]) {
            adjust = 1.0;
        } else {
            switch (settings.gainCurves[RUDDER_INDEX]) {
                case GAIN_CURVE_NORMAL:
                    adjust = 1.0 - (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) / stickRanges[RUDDER_INDEX]);
                    break;
                case GAIN_CURVE_FLAT:
                    adjust = 1.0 - (0.5 * (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) / stickRanges[RUDDER_INDEX]));
                    break;
                case GAIN_CURVE_STEEP:
                    adjust = 1.0 - (2.0 * (abs(rawServoPositions[RUDDER] - channelCenters[RUDDER]) / stickRanges[RUDDER_INDEX]));
                    break;
            }
            if (adjust < 0.0) {
                adjust = 0.0;
            }
        }
        error *= adjust;
        //Adjust for stick motion
        adjust = (1.0 - (abs(STICK_MOVE_FACTOR * dSticks[RUDDER_INDEX]) / stickRanges[RUDDER_INDEX]));
        if (adjust < 0.0) {
            adjust = 0.0;
        }
        error *= adjust;
        error = (error + 2.0 * lastYawError) / 3.0;
        dError = error - lastYawError;
        lastYawError = error;
        yawITerm += error;
        if (yawITerm > settings.yawPID._maxI) {
            yawITerm = settings.yawPID._maxI;
        } else if (yawITerm < -settings.yawPID._maxI) {
            yawITerm = -settings.yawPID._maxI;
        }
        lastYawError = error;
        rpyCorrections[YAW_INDEX] += (error * settings.yawPID._P
                + yawITerm * settings.yawPID._I
                + dError * settings.yawPID._D) * yawGains[NORMAL_GAIN];
    }
}
