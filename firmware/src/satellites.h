/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: satellites.h                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: satellite rx functions             //
/////////////////////////////////////////////////////

#ifndef SATELLITES_H
#define	SATELLITES_H

#ifdef	__cplusplus
extern "C" {
#endif

    enum {
        SAT1 = 0, SAT2 = 1, SAT3 = 2
    };
    void initSatellites(void);
    void satPowerOn(bool powerOn);
    bool bindSats(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SATELLITES_H */

