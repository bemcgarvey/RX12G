/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: launchAssist.h                            //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: launch assist mode                 //
/////////////////////////////////////////////////////

#ifndef LAUNCHASSIST_H
#define	LAUNCHASSIST_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initLaunchAssist(void);
    void launchAssistCalculate(int axes);

#ifdef	__cplusplus
}
#endif

#endif	/* LAUNCHASSIST_H */

