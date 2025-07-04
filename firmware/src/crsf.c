/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: crsf.c                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: ExpressLRS (CRSF) functions        //
/////////////////////////////////////////////////////
#include "definitions.h"
#include "crsf.h"
#include "rtosHandles.h"
#include "timers.h"
#include "settings.h"
#include "uart.h"
#include "satellites.h"
#include "gyroTask.h"

volatile __attribute__((coherent, aligned(4))) uint8_t crsfPacket[MAX_CRSF_PACKET];
volatile __attribute__((coherent, aligned(4))) uint8_t telemetryPacket[MAX_CRSF_PACKET];
volatile static uint8_t synched;
volatile static uint8_t received;
static uint8_t _lut[256];

void initCRC(uint8_t poly);

void initCRSF(void) {
    synched = false;
    received = 0;
    initCRC(0xd5);
    //ELRS is configured differently from other sats.
    //Uses only UART5 for tx and rx at 420000 baud
    U5BRG = 35; //420000 baud
    U5MODEbits.BRGH = 1;
    //SAT2 = UART5 RX
    //SAT1 = UART5 TX
    U5STAbits.URXEN = 1;
    U5STAbits.UTXEN = 1;
    //remap tx pin
    TRISBbits.TRISB0 = 0;
    RPB0R = 0b01011; //U5TX     
    U5STAbits.OERR = 0;
    U5MODEbits.ON = 1;
    EVIC_SourceEnable(INT_SOURCE_UART5_RX);
    EVIC_SourceEnable(INT_SOURCE_UART5_FAULT);
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_1, CRSFPacketDone, 0);
    DCH1ECONbits.CHAIRQ = INT_SOURCE_UART5_FAULT;
    DCH1ECONbits.AIRQEN = 1;
    //Setup addresses
    DCH1SSA = (uint32_t) KVA_TO_PA((uint32_t) & U5RXREG);
    DCH1DSA = (uint32_t) KVA_TO_PA((uint32_t) & crsfPacket[2]);
    DCH1SSIZ = 1;
    DCH1CSIZ = 1;
    TMR7_PeriodSet(1500); //1ms may need to go shorter for fast packet rates 1500 would be 100us
    TMR7_CallbackRegister(startCRSFPacket, 0);
    TMR7_Start();
    CRSF_Frame *frame = (CRSF_Frame *) telemetryPacket;
    frame->address = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    //TODO setup DMA for telemetry. Use DMA5 - check PMD to make sure it is enabled
}

void CRSF_RX_InterruptHandler(void) {
    uint8_t rx = U5RXREG;
    TMR7 = 0;
    if (synched) {
        crsfPacket[received] = rx;
        ++received;
        if (received == 2) {
            if (rx > 2 && rx <= MAX_CRSF_PACKET) {
                EVIC_SourceDisable(INT_SOURCE_UART5_RX);
                DCH1DSIZ = rx;
                DCH1CONbits.CHEN = 1;
            } else { //bad size
                synched = false;
                received = 0;
                TMR7_Start();
            }
        }
    }
}

void CRSFPacketDone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle) {
    if (status == DMAC_TRANSFER_EVENT_COMPLETE) {
        if (crsfPacket[2] == CRSF_FRAMETYPE_RC_CHANNELS_PACKED 
                && crsfPacket[0] == CRSF_ADDRESS_FLIGHT_CONTROLLER) {
            validPacketReceived = true;
            lastRxTime[SAT2] = getSystemTime();
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            //Send packet to rxTask, skip over the header
            xQueueSendToBackFromISR(rxQueue, (void *) &crsfPacket[3], &pxHigherPriorityTaskWoken);
            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (status == DMAC_TRANSFER_EVENT_ERROR) {
        DCH1ECONbits.CABORT = 1;
    }
    TMR7 = 0;
    TMR7_Start();
    EVIC_SourceEnable(INT_SOURCE_UART5_RX);
}

void startCRSFPacket(uint32_t status, uintptr_t context) {
    TMR7_Stop();
    synched = true;
    received = 0;
}

void initCRC(uint8_t poly) {
    for (int idx = 0; idx < 256; ++idx) {
        uint8_t crc = idx;
        for (int shift = 0; shift < 8; ++shift) {
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        _lut[idx] = crc & 0xff;
    }
}

uint8_t calcCRC(uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    while (len--) {
        crc = _lut[crc ^ *data++];
    }
    return crc;
}

void processCRSFPacket(uint8_t *data) {
    CRSF_ChannelData *ch = (CRSF_ChannelData*) data;
    rawServoPositions[0] = ch->ch0;
    rawServoPositions[1] = ch->ch1;
    rawServoPositions[2] = ch->ch2;
    rawServoPositions[3] = ch->ch3;
    rawServoPositions[4] = ch->ch4;
    rawServoPositions[5] = ch->ch5;
    rawServoPositions[6] = ch->ch6;
    rawServoPositions[7] = ch->ch7;
    rawServoPositions[8] = ch->ch8;
    rawServoPositions[9] = ch->ch9;
    rawServoPositions[10] = ch->ch10;
    rawServoPositions[11] = ch->ch11;
    rawServoPositions[12] = ch->ch12;
    rawServoPositions[13] = ch->ch13;
    rawServoPositions[14] = ch->ch14;
    rawServoPositions[15] = ch->ch15;
}

void sendModeTelemetry(void) {

}

void sendAttitudeTelemetry(void) {

}