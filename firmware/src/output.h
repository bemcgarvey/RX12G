/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: output.h                                  //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: servo output functions             //
/////////////////////////////////////////////////////

#ifndef OUTPUT_H
#define	OUTPUT_H

#include "timers.h"
#include "settings.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
#define NUM_OUTPUTS     12
#define OFFSET          (2097 * US_COUNT)
#define PULSE           (903 * US_COUNT)

    enum {
        THROTTLE = 0, AILERON = 1, ELEVATOR = 2, RUDDER = 3, GEAR = 4, AUX1 = 5,
        AUX2 = 6, AUX3 = 7, AUX4 = 8, AUX5 = 9, AUX6 = 10, AUX7 = 11
    };
    
    extern volatile uint16_t outputServos[MAX_CHANNELS];
    extern volatile bool failsafeEngaged;
    extern volatile bool outputsDisabled;
    
    void initOutputs(void);
    void disableThrottle(void);
    void engageFailsafe(void);
    void disableOutputs(int key);
    #define DISABLE_KEY         0x12983476

#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUT_H */

