/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: autoLevel.c                               //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: auto level mode                    //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "autoLevel.h"
#include "gyroTask.h"
#include "attitude.h"

static bool started;

void initAutoLevel(void) {
    centerCount = CENTER_COUNT;
    started = false;
}

void autoLevelCalculate(int axes) {
    float error;
    float deltaError;
    if (sticksCentered()) {
        if (axes & ROLL_AXIS) {
            error = -attitude.ypr.roll;
            if (started) {
                deltaError = error - lastRollError;
            } else {
                deltaError = 0;
                started = true;
            }
            rollITerm += error;
            if (rollITerm > settings.rollPID._maxI) {
                rollITerm = settings.rollPID._maxI;
            } else if (rollITerm < -settings.rollPID._maxI) {
                rollITerm = -settings.rollPID._maxI;
            }
            lastRollError = error;
            rpyCorrections[ROLL_INDEX] += (error * settings.rollPID._P
                    + rollITerm * settings.rollPID._I
                    + deltaError * settings.rollPID._D) * rollGains[LEVEL_GAIN];
        }
        if (axes & PITCH_AXIS) {
            error = -attitude.ypr.pitch;
            if (started) {
                deltaError = error - lastPitchError;
            } else {
                deltaError = 0;
                started = true;
            }
            pitchITerm += error;
            if (pitchITerm > settings.pitchPID._maxI) {
                pitchITerm = settings.pitchPID._maxI;
            } else if (pitchITerm < -settings.pitchPID._maxI) {
                pitchITerm = -settings.pitchPID._maxI;
            }
            lastPitchError = error;
            rpyCorrections[PITCH_INDEX] += (error * settings.pitchPID._P
                    + pitchITerm * settings.pitchPID._I
                    + deltaError * settings.pitchPID._D)
                    * cos(attitude.ypr.roll * DEGREES_TO_RAD) * pitchGains[LEVEL_GAIN];
            //cos term above corrects for roll - this will reverse correction when inverted
            //and reduce correction the closer we are to 90 degrees of roll
        }
    } else {
        started = false;
        if (axes & ROLL_AXIS) {
            rollITerm = 0;
        }
        if (axes & PITCH_AXIS) {
            pitchITerm = 0;
        }
    }
}
