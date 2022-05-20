/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: timers.c                                  //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: timer functions                    //
/////////////////////////////////////////////////////

#include "definitions.h"
#include "timers.h"

volatile uint32_t systemTime = 0;

void systemTick(uint32_t status, uintptr_t context) {
    ++systemTime;
}

void startSystemTime(void) {
    TMR9_CallbackRegister(systemTick, 0);
    TMR9_Start();
}

uint32_t getSystemTime(void) {
    return systemTime;
}