/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: rxTask.h                                  //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: packet rx rtos task                //
/////////////////////////////////////////////////////

#ifndef RXTASK_H
#define	RXTASK_H

#ifdef	__cplusplus
extern "C" {
#endif

void rxTask(void *pvParameters);


#ifdef	__cplusplus
}
#endif

#endif	/* RXTASK_H */

