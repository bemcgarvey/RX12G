/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: uart.h                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: uart functions                     //
/////////////////////////////////////////////////////

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    extern volatile bool validPacketReceived;
    extern volatile uint32_t lastRxTime[3];
    void initUARTs(bool detected[3]);
    void disableUARTs(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

