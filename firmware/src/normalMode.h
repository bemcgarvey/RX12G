/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: normalMode.h                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: normal rate mode                   //
/////////////////////////////////////////////////////

#ifndef NORMALMODE_H
#define	NORMALMODE_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initNormalMode(void);
    void normalModeCalculate(int axes);


#ifdef	__cplusplus
}
#endif

#endif	/* NORMALMODE_H */

