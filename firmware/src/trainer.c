#include "definitions.h"
#include "autoLevel.h"
#include "gyroTask.h"
#include "attitude.h"
#include "trainer.h"

static bool rollOutOfBounds;
static float rollSum;
static int rollDirection;

#define ERROR_SCALE_FACTOR  0.2

void initTrainerMode(void) {
    rollOutOfBounds = false;
}

//TODO finish trainer mode

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
            }
        }
    }
}
