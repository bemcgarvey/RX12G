
#include "launchAssist.h"
#include "gyroTask.h"
#include "settings.h"
#include "autoLevel.h"
#include "attitude.h"
#include "output.h"
#include "rxTask.h"
#include <stdlib.h>

static bool takeoffDone;

void initLaunchAssist(void) {
    takeoffDone = false;
}

void launchAssistCalculate(int axes) {
    float error;
    float deltaError;
    if ((abs(rawServoPositions[AILERON] - channelCenters[AILERON]) > deadbands[ROLL_INDEX])
            || (abs(rawServoPositions[ELEVATOR] - channelCenters[ELEVATOR]) > deadbands[ROLL_INDEX])) {
        takeoffDone = true;
    }
    if (axes & PITCH_AXIS) {
        if (!takeoffDone) {
            error = settings.takeoffPitch - attitude.ypr.pitch;
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
                    + deltaError * settings.pitchPID._D) * pitchGain;
            //Correct for roll - this will reverse correction when inverted
            //and reduce correction the closer we are to 90 degrees of roll.
            //This should only be a factor if the launch is way off from level
            rpyCorrections[PITCH_INDEX] *= cos(attitude.ypr.roll * DEGREES_TO_RAD);
        } else {
            autoLevelCalculate(PITCH_AXIS);
        }
    }
}