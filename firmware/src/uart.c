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

void initUARTs(void) {
    sat1Packet[16] = SAT1;
    sat2Packet[16] = SAT2;
    sat3Packet[16] = SAT3;
    //SAT1 = UART4
    U4BRG = 129; //115200 baud
    U4MODEbits.BRGH = 1;
    U4STAbits.URXEN = 1;
    U4STAbits.OERR = 0;

    /* //SAT2 = UART5
    U5BRG = 129; //115200 baud
    U5MODEbits.BRGH = 1;
    U5STAbits.URXEN = 1;
    IPC17bits.U5RXIP = 3;
    IPC17bits.U5RXIS = 3;
    IFS2bits.U5RXIF = 0;
    IEC2bits.U5RXIE = 1;
    IPC17bits.U5EIP = 4;
    IPC17bits.U5EIS = 3;
    IFS2bits.U5EIF = 0;
    IEC2bits.U5EIE = 1;
    U5STAbits.OERR = 0;

    //SAT3 = UART6
    U6BRG = 129; //115200 baud
    U6MODEbits.BRGH = 1;
    U6STAbits.URXEN = 1;
    IPC41bits.U6RXIP = 3;
    IPC41bits.U6RXIS = 2;
    IFS5bits.U6RXIF = 0;
    IEC5bits.U6RXIE = 1;
    IPC41bits.U6EIP = 4;
    IPC41bits.U6EIS = 2;
    IFS5bits.U6EIF = 0;
    IEC5bits.U6EIE = 1;
    U6STAbits.OERR = 0;  */

    U4MODEbits.ON = 1;
    EVIC_SourceEnable(INT_SOURCE_UART4_RX);
    EVIC_SourceEnable(INT_SOURCE_UART4_FAULT);
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, sat1DMADone, 0);
    //DCH0ECONbits.CHAIRQ = INT_SOURCE_UART4_FAULT;
    //DCH0ECONbits.AIRQEN = 1;
    TMR8_CallbackRegister(startSat1DMA, 0);
    TMR8_Start();
    //U5MODEbits.ON = 1;
    //U6MODEbits.ON = 1;
}

void startSat1DMA(uint32_t status, uintptr_t context) {
    TMR8_Stop();
    EVIC_SourceDisable(INT_SOURCE_UART4_RX);
    bool success = DMAC_ChannelTransfer(DMAC_CHANNEL_0, (const void *) &U4RXREG, 1, (const void *) sat1Packet, 16, 1);
    if (!success) {
        TMR8_Start();
        EVIC_SourceEnable(INT_SOURCE_UART4_RX);
    }
}

volatile int count = 0;

void sat1DMADone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle) {
    if (status == DMAC_TRANSFER_EVENT_COMPLETE) {
        //TODO put packet in queue
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
            if (ch == 1) {
                if (pos < 1024) {
                    SAT3_LED_Clear();
                } else {
                    SAT3_LED_Set();
                }
            } else if (ch == 2) {
                if (pos < 1024) {
                    SAT2_LED_Clear();
                } else {
                    SAT2_LED_Set();
                }
            }
        }
    }
    TMR8 = 0;
    TMR8_Start();
    EVIC_SourceEnable(INT_SOURCE_UART4_RX);
}