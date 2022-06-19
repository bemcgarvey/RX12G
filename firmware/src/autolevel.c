#include "definitions.h"
#include "autoLevel.h"
#include "gyroTask.h"
#include "attitude.h"

static float lastRollError;
static float rollITerm;
static float lastPitchError;
static float pitchITerm;

void initAutoLevel(void) {
    rollITerm = 0;
    pitchITerm = 0;
    lastRollError = 0;
    lastPitchError = 0;
    centerCount = CENTER_COUNT;
}

void autoLevelCalculate(int axes) {
    float error;
    float deltaError;
    if (sticksCentered()) {
        if (axes & ROLL_AXIS) {
            error = -attitude.ypr.roll;
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
        if (axes & PITCH_AXIS) {
            if (attitude.zSign < 0) {
                error = attitude.ypr.pitch; //upside down so pitch is reversed
            } else {
                error = -attitude.ypr.pitch;
            }
            deltaError = error - lastPitchError;
            if ((error > 0 && lastPitchError < 0) || (error < 0 && lastPitchError > 0)) {
                pitchITerm = 0;
            } else {
                pitchITerm += error;
                if (pitchITerm > settings.pitchPID._maxI) {
                    pitchITerm = settings.pitchPID._maxI;
                } else if (pitchITerm < -settings.pitchPID._maxI) {
                    pitchITerm = -settings.pitchPID._maxI;
                }
            }
            lastPitchError = error;
            rpyCorrections[PITCH_INDEX] = error * settings.pitchPID._P * pitchGain
                    + pitchITerm * settings.pitchPID._I * pitchGain
                    + deltaError * settings.pitchPID._D * pitchGain;
        }
    } else {
        rollITerm = 0;
        pitchITerm = 0;
        lastRollError = 0;
        lastPitchError = 0;
    }
}
