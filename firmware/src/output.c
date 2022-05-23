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

uint32_t pwmPeriod = 20; //TODO set this from saved settings

bool outputsActive = false;

uint16_t outputServos[MAX_CHANNELS];

const unsigned int startOffsets[NUM_OUTPUTS] = {0, OFFSET, 0, OFFSET, 0, OFFSET,
    0, OFFSET, 2 * OFFSET, 2 * OFFSET, 2 * OFFSET, 2 * OFFSET};

const unsigned int pulseOffsets[NUM_OUTPUTS] = {0 + PULSE, OFFSET + PULSE,
    0 + PULSE, OFFSET + PULSE, 0 + PULSE, OFFSET + PULSE, 0 + PULSE, OFFSET + PULSE,
    2 * OFFSET + PULSE, 2 * OFFSET + PULSE, 2 * OFFSET + PULSE, 2 * OFFSET + PULSE};

volatile unsigned int* const pulseRegister[NUM_OUTPUTS] = {&OC11RS, &OC10RS, &OC9RS,
    &OC13RS, &OC2RS, &OC6RS, &OC8RS, &OC5RS, &OC14RS, &OC1RS, &OC7RS, &OC4RS};

volatile unsigned int* const startRegister[NUM_OUTPUTS] = {&OC11R, &OC10R, &OC9R,
    &OC13R, &OC2R, &OC6R, &OC8R, &OC5R, &OC14R, &OC1R, &OC7R, &OC4R};

volatile unsigned int* const OCxCONSETRegister[NUM_OUTPUTS] = {&OC11CONSET, &OC10CONSET, &OC9CONSET,
    &OC13CONSET, &OC2CONSET, &OC6CONSET, &OC8CONSET, &OC5CONSET, &OC14CONSET, &OC1CONSET, &OC7CONSET, &OC4CONSET};

volatile unsigned int* const OCxCONCLRRegister[NUM_OUTPUTS] = {&OC11CONCLR, &OC10CONCLR, &OC9CONCLR,
    &OC13CONCLR, &OC2CONCLR, &OC6CONCLR, &OC8CONCLR, &OC5CONCLR, &OC14CONCLR, &OC1CONCLR, &OC7CONCLR, &OC4CONCLR};

void updatePulses(uint32_t status, uintptr_t context);

void initOutputs(void) {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        outputServos[i] = 0xffff;
    }
    //Ch 0 (zero based so this is actually Ch 1 on the Rx board)
    OC11CONbits.OC32 = 1;
    OC11CONbits.OCM = 0b101;
    //Ch 1
    OC10CONbits.OC32 = 1;
    OC10CONbits.OCM = 0b101;
    //Ch 2
    OC9CONbits.OC32 = 1;
    OC9CONbits.OCM = 0b101;
    //Ch 3
    OC13CONbits.OC32 = 1;
    OC13CONbits.OCM = 0b101;
    //Ch 4
    OC2CONbits.OC32 = 1;
    OC2CONbits.OCM = 0b101;
    //Ch 5
    OC6CONbits.OC32 = 1;
    OC6CONbits.OCM = 0b101;
    //Ch 6
    OC8CONbits.OC32 = 1;
    OC8CONbits.OCM = 0b101;
    //Ch 7
    OC5CONbits.OC32 = 1;
    OC5CONbits.OCM = 0b101;
    //Ch 8
    OC14CONbits.OC32 = 1;
    OC14CONbits.OCM = 0b101;
    //Ch 9
    OC1CONbits.OC32 = 1;
    OC1CONbits.OCM = 0b101;
    //Ch 10
    OC7CONbits.OC32 = 1;
    OC7CONbits.OCM = 0b101;
    //Ch 11
    OC4CONbits.OC32 = 1;
    OC4CONbits.OCM = 0b101;
    for (int i = 0; i < NUM_OUTPUTS; ++i) {
        *startRegister[i] = startOffsets[i];
    }
    TMR2_PeriodSet(pwmPeriod * MS_COUNT - 1);
    TMR2_CallbackRegister(updatePulses, 0);
}

void enableActiveOutputs(void) {
    for (int i = 0; i < NUM_OUTPUTS; ++i) {
        if (outputServos[i] != 0xffff) {
            *OCxCONSETRegister[i] = 0x8000; //ON bit
        }
    }
    TMR2_Start();
    outputsActive = true;
    //TODO make sure first pulse is correct
}

void disableThrottle(void) {
    *OCxCONCLRRegister[THROTTLE] = 0x8000; //clear ON bit
}

void enableThrottle(void) {
    *OCxCONSETRegister[THROTTLE] = 0x8000; //set ON bit
}

void updatePulses(uint32_t status, uintptr_t context) {
    for (int i = 0; i < NUM_OUTPUTS; ++i) {
        uint32_t out = ((1194 * US_COUNT) * outputServos[i]) / 2048;
        *pulseRegister[i] = out + pulseOffsets[i];
    }
}

//TODO handle S.Bus here based on outputServos[]