/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: sbus.h                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: sbus functions - in and out        //
/////////////////////////////////////////////////////

#ifndef SBUS_H
#define	SBUS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SBUS_HEADER     0x0f
#define SBUS_FOOTER     0x00
#define SBUS_FAILSAFE   0x08
    
    void initSBus(void);
    void processSBusPacket(uint8_t *buffer);

#ifdef	__cplusplus
}
#endif

#endif	/* SBUS_H */

