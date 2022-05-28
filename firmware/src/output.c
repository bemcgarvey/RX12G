/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: output.c                                  //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: servo output functions             //
/////////////////////////////////////////////////////
#include "definitions.h"
#include "output.h"
#include "timers.h"
#include "settings.h"
#include "sbus.h"

volatile uint16_t outputServos[MAX_CHANNELS];
volatile bool failsafeEngaged = false;
int numPWMOutputs = NUM_OUTPUTS;

const unsigned int startOffsets[NUM_OUTPUTS] = {0, OFFSET, 0, OFFSET, 0, OFFSET,
    0, OFFSET, 2 * OFFSET, 2 * OFFSET, 2 * OFFSET, 2 * OFFSET};

const unsigned int pulseOffsets[NUM_OUTPUTS] = {0 + PULSE, OFFSET + PULSE,
    0 + PULSE, OFFSET + PULSE, 0 + PULSE, OFFSET + PULSE, 0 + PULSE, OFFSET + PULSE,
    2 * OFFSET + PULSE, 2 * OFFSET + PULSE, 2 * OFFSET + PULSE, 2 * OFFSET + PULSE};

//TODO may need to adjust upper channel offsets for higher servo rates

volatile unsigned int* const pulseRegister[NUM_OUTPUTS] = {&OC11RS, &OC10RS, &OC9RS,
    &OC13RS, &OC2RS, &OC6RS, &OC8RS, &OC5RS, &OC14RS, &OC1RS, &OC7RS, &OC4RS};

volatile unsigned int* const startRegister[NUM_OUTPUTS] = {&OC11R, &OC10R, &OC9R,
    &OC13R, &OC2R, &OC6R, &OC8R, &OC5R, &OC14R, &OC1R, &OC7R, &OC4R};

volatile unsigned int* const OCxCONSETRegister[NUM_OUTPUTS] = {&OC11CONSET, &OC10CONSET, &OC9CONSET,
    &OC13CONSET, &OC2CONSET, &OC6CONSET, &OC8CONSET, &OC5CONSET, &OC14CONSET, &OC1CONSET, &OC7CONSET, &OC4CONSET};

volatile unsigned int* const OCxCONCLRRegister[NUM_OUTPUTS] = {&OC11CONCLR, &OC10CONCLR, &OC9CONCLR,
    &OC13CONCLR, &OC2CONCLR, &OC6CONCLR, &OC8CONCLR, &OC5CONCLR, &OC14CONCLR, &OC1CONCLR, &OC7CONCLR, &OC4CONCLR};

volatile unsigned int* const OCxCONRegister[NUM_OUTPUTS] = {&OC11CON, &OC10CON, &OC9CON,
    &OC13CON, &OC2CON, &OC6CON, &OC8CON, &OC5CON, &OC14CON, &OC1CON, &OC7CON, &OC4CON};

void updatePulses(uint32_t status, uintptr_t context);

void initOutputs(void) {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        outputServos[i] = 0xffff;
    }
    //Channels are zero based in firmware but start at 1 on board
    for (int i = 0; i < NUM_OUTPUTS; ++i) {
        *OCxCONSETRegister[i] = 0x0025;  //OC32 = 1, OCM = 0b101
        *startRegister[i] = startOffsets[i];
    }
    numPWMOutputs = NUM_OUTPUTS - settings.numSBusOutputs;
    TMR2_PeriodSet(((1000 / settings.outputHz) * MS_COUNT) - 1);
    TMR2_CallbackRegister(updatePulses, 0);
    TMR2_Start();
    initSBus();
}

void disableThrottle(void) {
    outputServos[THROTTLE] = 0xffff;
    *OCxCONCLRRegister[THROTTLE] = 0x8000;
}

void engageFailsafe(void) {
    if (settings.failsafeType == NORMAL_FAILSAFE) {
        disableThrottle();
    } else if (settings.failsafeType == PRESET_FAILSAFE) {
        for (int i = 0; i < MAX_CHANNELS; ++i) {
            outputServos[i] = channelPresets[i];
        }
    }
    failsafeEngaged = true;
}

void updatePulses(uint32_t status, uintptr_t context) {
    for (int i = 0; i < numPWMOutputs; ++i) {
        if (outputServos[i] != 0xffff && !failsafeEngaged) {
            if (!(*OCxCONRegister[i] & 0x8000)) {
                *OCxCONSETRegister[i] = 0x8000; //set ON bit
            }
        }
        uint32_t out = ((1194 * US_COUNT) * outputServos[i]) / 2048;
        *pulseRegister[i] = out + pulseOffsets[i];
    }
}
