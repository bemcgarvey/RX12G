/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: uart.h                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: uart functions                     //
/////////////////////////////////////////////////////
#include "definitions.h"
#include "uart.h"
#include "satellites.h"
#include "rtosHandles.h"
#include "timers.h"
#include "settings.h"
#include "sbus.h"

static volatile __attribute__((coherent, aligned(4))) uint8_t sat1Packet[25];
static volatile __attribute__((coherent, aligned(4))) uint8_t sat2Packet[25];
static volatile __attribute__((coherent, aligned(4))) uint8_t sat3Packet[25];

volatile bool validPacketReceived = false;
volatile uint32_t lastRxTime[3] = {0, 0, 0};

void startSat1DMA(uint32_t status, uintptr_t context);
void sat1DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);
void startSat2DMA(uint32_t status, uintptr_t context);
void sat2DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);
void startSat3DMA(uint32_t status, uintptr_t context);
void sat3DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);

void initUARTs(bool detected[3]) {
    validPacketReceived = false;
    if (settings.satType == SAT_TYPE_SBUS) {
        U4BRG = 149; //100000 baud
        U4MODEbits.BRGH = 1;
        U5BRG = 149; //100000 baud
        U5MODEbits.BRGH = 1;
        U6BRG = 149; //100000 baud
        U6MODEbits.BRGH = 1;
        U4MODEbits.PDSEL = 0b01; //8E
        U4MODEbits.STSEL = 1; //2 stop bits
        U4MODEbits.RXINV = 1; //invert
        U5MODEbits.PDSEL = 0b01; //8E
        U5MODEbits.STSEL = 1; //2 stop bits
        U5MODEbits.RXINV = 1; //invert
        U6MODEbits.PDSEL = 0b01; //8E
        U6MODEbits.STSEL = 1; //2 stop bits
        U6MODEbits.RXINV = 1; //invert
    } else {
        U4BRG = 129; //115200 baud
        U4MODEbits.BRGH = 1;
        U5BRG = 129; //115200 baud
        U5MODEbits.BRGH = 1;
        U6BRG = 129; //115200 baud
        U6MODEbits.BRGH = 1;
    }
    //SAT1 = UART4
    U4STAbits.URXEN = 1;
    U4STAbits.OERR = 0;
    //SAT2 = UART5
    U5STAbits.URXEN = 1;
    U5STAbits.OERR = 0;
    //SAT3 = UART6
    U6STAbits.URXEN = 1;
    U6STAbits.OERR = 0;
    int pSize = 16;
    if (settings.satType == SAT_TYPE_SBUS) {
        pSize = 25;
    }
    if (detected[SAT1]) {
        U4MODEbits.ON = 1;
        EVIC_SourceEnable(INT_SOURCE_UART4_RX);
        EVIC_SourceEnable(INT_SOURCE_UART4_FAULT);
        DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, sat1DMADone, 0);
        DCH0ECONbits.CHAIRQ = INT_SOURCE_UART4_FAULT;
        DCH0ECONbits.AIRQEN = 1;
        //Setup addresses
        DCH0SSA = (uint32_t) KVA_TO_PA((uint32_t) & U4RXREG);
        DCH0DSA = (uint32_t) KVA_TO_PA((uint32_t) sat1Packet);
        DCH0SSIZ = 1;
        DCH0DSIZ = pSize;
        DCH0CSIZ = 1;
        TMR8_CallbackRegister(startSat1DMA, 0);
        TMR8_Start();
    }
    if (detected[SAT2]) {
        U5MODEbits.ON = 1;
        EVIC_SourceEnable(INT_SOURCE_UART5_RX);
        EVIC_SourceEnable(INT_SOURCE_UART5_FAULT);
        DMAC_ChannelCallbackRegister(DMAC_CHANNEL_1, sat2DMADone, 0);
        DCH1ECONbits.CHAIRQ = INT_SOURCE_UART5_FAULT;
        DCH1ECONbits.AIRQEN = 1;
        //Setup addresses
        DCH1SSA = (uint32_t) KVA_TO_PA((uint32_t) & U5RXREG);
        DCH1DSA = (uint32_t) KVA_TO_PA((uint32_t) sat2Packet);
        DCH1SSIZ = 1;
        DCH1DSIZ = pSize;
        DCH1CSIZ = 1;
        TMR7_CallbackRegister(startSat2DMA, 0);
        TMR7_Start();
    }
    if (detected[SAT3]) {
        U6MODEbits.ON = 1;
        EVIC_SourceEnable(INT_SOURCE_UART6_RX);
        EVIC_SourceEnable(INT_SOURCE_UART6_FAULT);
        DMAC_ChannelCallbackRegister(DMAC_CHANNEL_2, sat3DMADone, 0);
        DCH2ECONbits.CHAIRQ = INT_SOURCE_UART6_FAULT;
        DCH2ECONbits.AIRQEN = 1;
        //Setup addresses
        DCH2SSA = (uint32_t) KVA_TO_PA((uint32_t) & U6RXREG);
        DCH2DSA = (uint32_t) KVA_TO_PA((uint32_t) sat3Packet);
        DCH2SSIZ = 1;
        DCH2DSIZ = pSize;
        DCH2CSIZ = 1;
        TMR6_CallbackRegister(startSat3DMA, 0);
        TMR6_Start();
    }
}

void disableUARTs(void) {
    U4MODEbits.ON = 0;
    U5MODEbits.ON = 0;
    U6MODEbits.ON = 0;
    TMR6_Stop();
    TMR7_Stop();
    TMR8_Stop();
    DCH0ECONbits.CABORT = 1;
    DCH1ECONbits.CABORT = 1;
    DCH2ECONbits.CABORT = 1;
}

void startSat1DMA(uint32_t status, uintptr_t context) {
    TMR8_Stop();
    EVIC_SourceDisable(INT_SOURCE_UART4_RX);
    DCH0CONbits.CHEN = 1;
}

void sat1DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle) {
    if (status == DMAC_TRANSFER_EVENT_COMPLETE) {
        if (settings.satType == SAT_TYPE_DSMX
                || (sat1Packet[0] == SBUS_HEADER && sat1Packet[24] == SBUS_FOOTER
                && !(sat1Packet[23] & SBUS_FAILSAFE))) {
            validPacketReceived = true;
            lastRxTime[SAT1] = getSystemTime();
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            xQueueSendToBackFromISR(rxQueue, (void *) sat1Packet, &pxHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        DCH0ECONbits.CABORT = 1;
    }
    TMR8 = 0;
    TMR8_Start();
    EVIC_SourceEnable(INT_SOURCE_UART4_RX);
}

void startSat2DMA(uint32_t status, uintptr_t context) {
    TMR7_Stop();
    EVIC_SourceDisable(INT_SOURCE_UART5_RX);
    DCH1CONbits.CHEN = 1;
}

void sat2DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle) {
    if (status == DMAC_TRANSFER_EVENT_COMPLETE) {
        if (settings.satType == SAT_TYPE_DSMX
                || (sat2Packet[0] == SBUS_HEADER && sat2Packet[24] == SBUS_FOOTER
                && !(sat2Packet[23] & SBUS_FAILSAFE))) {
            validPacketReceived = true;
            lastRxTime[SAT2] = getSystemTime();
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            xQueueSendToBackFromISR(rxQueue, (void *) sat2Packet, &pxHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        DCH1ECONbits.CABORT = 1;
    }
    TMR7 = 0;
    TMR7_Start();
    EVIC_SourceEnable(INT_SOURCE_UART5_RX);
}

void startSat3DMA(uint32_t status, uintptr_t context) {
    TMR6_Stop();
    EVIC_SourceDisable(INT_SOURCE_UART6_RX);
    DCH2CONbits.CHEN = 1;
}

void sat3DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle) {
    if (status == DMAC_TRANSFER_EVENT_COMPLETE) {
        if (settings.satType == SAT_TYPE_DSMX
                || (sat3Packet[0] == SBUS_HEADER && sat3Packet[24] == SBUS_FOOTER
                && !(sat3Packet[23] & SBUS_FAILSAFE))) {
            validPacketReceived = true;
            lastRxTime[SAT3] = getSystemTime();
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            xQueueSendToBackFromISR(rxQueue, (void *) sat3Packet, &pxHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        DCH2ECONbits.CABORT = 1;
    }
    TMR6 = 0;
    TMR6_Start();
    EVIC_SourceEnable(INT_SOURCE_UART6_RX);
}