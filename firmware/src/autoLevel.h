/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: autoLevel.h                               //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: auto level mode                    //
/////////////////////////////////////////////////////

#ifndef AUTOLEVEL_H
#define	AUTOLEVEL_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initAutoLevel(void);
    void autoLevelCalculate(int axes);

#ifdef	__cplusplus
}
#endif

#endif	/* AUTOLEVEL_H */

