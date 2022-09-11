/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: rxOnlyTask.h                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: task for rx functions only (no imu)//
/////////////////////////////////////////////////////

#ifndef RXONLYTASK_H
#define	RXONLYTASK_H

#ifdef	__cplusplus
extern "C" {
#endif


void rxOnlyTask(void *pvParameters);


#ifdef	__cplusplus
}
#endif

#endif	/* RXONLYTASK_H */

