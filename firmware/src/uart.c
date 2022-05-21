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

static volatile __attribute__((coherent)) uint8_t sat1Packet[17];
static volatile __attribute__((coherent)) uint8_t sat2Packet[17];
static volatile __attribute__((coherent)) uint8_t sat3Packet[17];

void startSat1DMA(uint32_t status, uintptr_t context);
void sat1DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);
void startSat2DMA(uint32_t status, uintptr_t context);
void sat2DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);
void startSat3DMA(uint32_t status, uintptr_t context);
void sat3DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);

void initUARTs(bool detected[3]) {
    sat1Packet[16] = SAT1;
    sat2Packet[16] = SAT2;
    sat3Packet[16] = SAT3;
    //SAT1 = UART4
    U4BRG = 129; //115200 baud
    U4MODEbits.BRGH = 1;
    U4STAbits.URXEN = 1;
    U4STAbits.OERR = 0;
    //SAT2 = UART5
    U5BRG = 129; //115200 baud
    U5MODEbits.BRGH = 1;
    U5STAbits.URXEN = 1;
    U5STAbits.OERR = 0;
    //SAT3 = UART6
    U6BRG = 129; //115200 baud
    U6MODEbits.BRGH = 1;
    U6STAbits.URXEN = 1;
    U6STAbits.OERR = 0;

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
        DCH0DSIZ = 16;
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
        DCH1DSIZ = 16;
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
        DCH2DSIZ = 16;
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

volatile int count = 0; //TODO remove

void sat1DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle) {
    if (status == DMAC_TRANSFER_EVENT_COMPLETE) {
        //TODO put packet in queue

        //TODO remove below after testing
        ++count;
        if (count == 50) {
            LED_B_Toggle();
            count = 0;
        }
        uint16_t *p = (uint16_t *) sat1Packet;
        for (int i = 1; i < 8; ++i) {
            p[i] = p[i] >> 8 | p[i] << 8;
            uint16_t ch = p[i] >> 11;
            ch &= 0x000f;
            uint16_t pos = p[i] & 0x07ff;
            if (ch == 0) {
                if (pos < 1024) {
                    SAT1_LED_Clear();
                } else {
                    SAT1_LED_Set();
                }
            }
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        SAT1_LED_Set(); //TODO remove
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
        //TODO put packet in queue

        //TODO remove below after testing
        ++count;
        if (count == 50) {
            LED_B_Toggle();
            count = 0;
        }
        uint16_t *p = (uint16_t *) sat2Packet;
        for (int i = 1; i < 8; ++i) {
            p[i] = p[i] >> 8 | p[i] << 8;
            uint16_t ch = p[i] >> 11;
            ch &= 0x000f;
            uint16_t pos = p[i] & 0x07ff;
            if (ch == 1) {
                if (pos < 1024) {
                    SAT2_LED_Clear();
                } else {
                    SAT2_LED_Set();
                }
            } 
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        SAT1_LED_Set(); //TODO remove
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
        //TODO put packet in queue

        //TODO remove below after testing
        ++count;
        if (count == 50) {
            LED_B_Toggle();
            count = 0;
        }
        uint16_t *p = (uint16_t *) sat3Packet;
        for (int i = 1; i < 8; ++i) {
            p[i] = p[i] >> 8 | p[i] << 8;
            uint16_t ch = p[i] >> 11;
            ch &= 0x000f;
            uint16_t pos = p[i] & 0x07ff;
            if (ch == 2) {
                if (pos < 1024) {
                    SAT3_LED_Clear();
                } else {
                    SAT3_LED_Set();
                }
            } 
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        SAT1_LED_Set(); //TODO remove
        DCH2ECONbits.CABORT = 1;
    }
    TMR6 = 0;
    TMR6_Start();
    EVIC_SourceEnable(INT_SOURCE_UART6_RX);
}