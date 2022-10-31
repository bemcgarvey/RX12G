/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: angleMode.h                               //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: Angle rate mode                    //
/////////////////////////////////////////////////////

#ifndef ANGLEMODE_H
#define	ANGLEMODE_H

#ifdef	__cplusplus
extern "C" {
#endif

void initAngleMode(void);
void angleModeCalculate(int axes);

#ifdef	__cplusplus
}
#endif

#endif	/* ANGLEMODE_H */

