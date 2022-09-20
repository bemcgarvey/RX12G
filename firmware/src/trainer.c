#include "definitions.h"
#include "autoLevel.h"
#include "gyroTask.h"
#include "attitude.h"
#include "trainer.h"
#include "normalMode.h"

static bool rollOutOfBounds;
static float rollSum;
static int rollDirection;
static bool pitchOutOfBounds;
static float pitchSum;
static int pitchDirection;

void initTrainerMode(void) {
    rollOutOfBounds = false;
    pitchOutOfBounds = false;
}

void trainerModeCalculate(int axes) {
    float error;
    float deltaError;

    if (sticksCentered()) {
        autoLevelCalculate(axes);
    } else {
        if (axes & ROLL_AXIS) {
            if (rollOutOfBounds) {
                rollSum += attitude.gyroRatesDeg.rollRate;
                if ((rollDirection > 0 && rollSum <= 0) || (rollDirection < 0 && rollSum >= 0)) {
                    rollOutOfBounds = false;
                } else {
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
            } else if (attitude.ypr.roll > settings.rollLimit || attitude.ypr.roll < -settings.rollLimit) {
                lastRollError = 0;
                rollITerm = 0;
                rollOutOfBounds = true;
                rollSum = 0;
                if (attitude.ypr.roll > 0) {
                    rollDirection = 1;
                } else {
                    rollDirection = -1;
                }
            } else {
                normalModeCalculate(ROLL_AXIS);
            }
        }
        if (axes & PITCH_AXIS) {
            if (pitchOutOfBounds) {
                pitchSum += attitude.gyroRatesDeg.pitchRate;
                if ((pitchDirection > 0 && pitchSum <= 0) || (pitchDirection < 0 && pitchSum >= 0)) {
                    pitchOutOfBounds = false;
                } else {
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
            } else if (attitude.ypr.pitch > settings.pitchLimit || attitude.ypr.pitch < -settings.pitchLimit) {
                lastPitchError = 0;
                pitchITerm = 0;
                pitchOutOfBounds = true;
                pitchSum = 0;
                if (attitude.ypr.pitch > 0) {
                    pitchDirection = 1;
                } else {
                    pitchDirection = -1;
                }
            } else {
                normalModeCalculate(PITCH_AXIS);
            }
        }
    }
}
