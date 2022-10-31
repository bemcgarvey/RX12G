/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: attitudeLock.h                            //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: attitude lock mode                 //
/////////////////////////////////////////////////////

#ifndef ATTITUDELOCK_H
#define	ATTITUDELOCK_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initAttitudeLock(void);
    void attitudeLockCalculate(int axes);


#ifdef	__cplusplus
}
#endif

#endif	/* ATTITUDELOCK_H */

