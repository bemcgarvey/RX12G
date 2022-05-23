/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: rtosHandles.h                             //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: handles for rtos objects           //
/////////////////////////////////////////////////////

#ifndef RTOSHANDLES_H
#define	RTOSHANDLES_H

#ifdef	__cplusplus
extern "C" {
#endif

    //Queues
    extern QueueHandle_t rxQueue;
    extern QueueHandle_t rawServoQueue;

    //Tasks
    extern TaskHandle_t buttonTaskHandle;
    extern TaskHandle_t rxTaskHandle;
    extern TaskHandle_t gyroTaskHandle;
    extern TaskHandle_t satLedTaskHandle;

#ifdef	__cplusplus
}
#endif

#endif	/* RTOSHANDLES_H */

