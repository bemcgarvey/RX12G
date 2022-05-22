/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: timers.h                                  //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: timer functions                    //
/////////////////////////////////////////////////////

#ifndef TIMERS_H
#define	TIMERS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MS_COUNT    ((CPU_CLOCK_FREQUENCY / 2) / 1000)
#define US_COUNT    ((CPU_CLOCK_FREQUENCY / 2) / 1000000)  

    void startSystemTime(void);
    uint32_t getSystemTime(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

