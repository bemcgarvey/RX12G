/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: buttonTask.h                              //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: bind button rtos task              //
/////////////////////////////////////////////////////

#ifndef BUTTONTASK_H
#define	BUTTONTASK_H

#ifdef	__cplusplus
extern "C" {
#endif

    void buttonTask(void *pvParameters);

#ifdef	__cplusplus
}
#endif

#endif	/* BUTTONTASK_H */

