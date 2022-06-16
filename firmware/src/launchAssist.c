
#include "launchAssist.h"
#include "gyroTask.h"
#include "settings.h"
#include "autoLevel.h"
#include "attitude.h"


static float lastPitchError;
static float pitchITerm;
static bool takeoffDone;

void initLaunchAssist(void) {
    pitchITerm = 0;
    lastPitchError = 0;
    takeoffDone = false;
}

void launchAssistCalculate(int axes) {
    float error;
    float deltaError;
    if (!sticksCentered()) {
        takeoffDone = true;
    }
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
        rpyCorrections[PITCH_INDEX] = error * settings.pitchPID._P * pitchGain
                + pitchITerm * settings.pitchPID._I * pitchGain
                + deltaError * settings.pitchPID._D * pitchGain;
    } else {
        autoLevelCalculate(axes);
    }
}