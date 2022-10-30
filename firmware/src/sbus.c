
#include "definitions.h"
#include "sbus.h"
#include "settings.h"
#include "output.h"
#include "gyroTask.h"

typedef union __attribute__((packed)) {

    struct __attribute__((packed)) {
        uint8_t header;
        uint8_t channels[22];
        uint8_t flags;
        uint8_t footer;
    };
    uint8_t bytes[25];
}
SBusPacket;

static volatile __attribute__((coherent, aligned(4))) SBusPacket sbusPacket;

void transmitSBusPacket(uint32_t status, uintptr_t context);

void initSBus(void) {
    if (settings.numSBusOutputs == 0) {
        return;
    }
    //initialize packet;
    for (int i = 0; i < 25; ++i) {
        sbusPacket.bytes[i] = 0;
    }
    sbusPacket.header = SBUS_HEADER;
    sbusPacket.footer = SBUS_FOOTER;
    //Setup UART3
    //100000 baud, 8E2, inverted polarity
    U3MODEbits.CLKSEL = 0b01; //SYSCLOCK
    U3BRG = 74; //100000 baud
    U3MODEbits.BRGH = 0;
    U3MODEbits.PDSEL = 0b01; //8E
    U3MODEbits.STSEL = 1; //2 stop bits
    U3STAbits.UTXINV = 1; //invert
    U3STAbits.UTXISEL = 0b10; //interrupt when tx buffer empty
    U3STAbits.UTXEN = 1;
    U3MODEbits.ON = 1;
    if (settings.numSBusOutputs > 1) {
        //Setup UART1
        //100000 baud, 8E2, inverted polarity
        U1MODEbits.CLKSEL = 0b01; //SYSCLOCK
        U1BRG = 74; //100000 baud
        U1MODEbits.BRGH = 0;
        U1MODEbits.PDSEL = 0b01; //8E
        U1MODEbits.STSEL = 1; //2 stop bits
        U1STAbits.UTXINV = 1; //invert
        U1STAbits.UTXISEL = 0b10; //interrupt when tx buffer empty
        U1STAbits.UTXEN = 1;
        U1MODEbits.ON = 1;
    }
    //Assign pins
    switch (settings.numSBusOutputs) {
        case 1:
            RPG8R = 1;
            break;
        case 2:
            RPG8R = 1;
            RPG6R = 1;
            break;
        case 3:
            RPG8R = 1;
            RPG6R = 1;
            RPB15R = 1;
            break;
        case 4:
            RPG8R = 1;
            RPG6R = 1;
            RPB15R = 1;
            RPA7R = 1;
            break;
    }

    //Setup DMA
    DCH3SSA = (uint32_t) KVA_TO_PA((uint32_t) & sbusPacket);
    DCH3DSA = (uint32_t) KVA_TO_PA((uint32_t) & U3TXREG);
    DCH3SSIZ = 25;
    DCH3DSIZ = 1;
    DCH3CSIZ = 1;

    if (settings.numSBusOutputs > 1) {
        DCH4SSA = (uint32_t) KVA_TO_PA((uint32_t) & sbusPacket);
        DCH4DSA = (uint32_t) KVA_TO_PA((uint32_t) & U1TXREG);
        DCH4SSIZ = 25;
        DCH4DSIZ = 1;
        DCH4CSIZ = 1;
    }
    TMR5_PeriodSet(TMR5_FrequencyGet() / 1000 * settings.sBusPeriodMs - 1);
    TMR5_CallbackRegister(transmitSBusPacket, 0);
    TMR5_Start();
}

void transmitSBusPacket(uint32_t status, uintptr_t context) {
    //build packet - Use Futaba AETR order
    //Note: Channels above 12 have not been tested.
    uint16_t adjustedServos[MAX_CHANNELS];
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (outputServos[i] == 0xffff) {
            adjustedServos[i] = 0;
        } else {
            adjustedServos[i] = outputServos[i] & 0x07ff;
        }
    }
    if (settings.channelOrder == CHANNEL_ORDER_AETR) {
        sbusPacket.channels[0] = adjustedServos[0];
        sbusPacket.channels[1] = (adjustedServos[0] >> 8) | (adjustedServos[1] << 3);
        sbusPacket.channels[2] = (adjustedServos[1] >> 5) | (adjustedServos[2] << 6);
        sbusPacket.channels[3] = (adjustedServos[2] >> 2);
        sbusPacket.channels[4] = (adjustedServos[2] >> 10) | (adjustedServos[3] << 1);
    } else {
        sbusPacket.channels[0] = adjustedServos[1];
        sbusPacket.channels[1] = (adjustedServos[1] >> 8) | (adjustedServos[2] << 3);
        sbusPacket.channels[2] = (adjustedServos[2] >> 5) | (adjustedServos[0] << 6);
        sbusPacket.channels[3] = (adjustedServos[0] >> 2);
        sbusPacket.channels[4] = (adjustedServos[0] >> 10) | (adjustedServos[3] << 1);
    }
    sbusPacket.channels[5] = (adjustedServos[3] >> 7) | (adjustedServos[4] << 4);
    sbusPacket.channels[6] = (adjustedServos[4] >> 4) | (adjustedServos[5] << 7);
    sbusPacket.channels[7] = (adjustedServos[5] >> 1);
    sbusPacket.channels[8] = (adjustedServos[5] >> 9) | (adjustedServos[6] << 2);
    sbusPacket.channels[9] = (adjustedServos[6] >> 6) | (adjustedServos[7] << 5);
    sbusPacket.channels[10] = (adjustedServos[7] >> 3);
    sbusPacket.channels[11] = adjustedServos[8];
    sbusPacket.channels[12] = (adjustedServos[8] >> 8) | (adjustedServos[9] << 3);
    sbusPacket.channels[13] = (adjustedServos[9] >> 5) | (adjustedServos[10] << 6);
    sbusPacket.channels[14] = (adjustedServos[10] >> 2);
    sbusPacket.channels[15] = (adjustedServos[10] >> 10) | (adjustedServos[11] << 1);
    sbusPacket.channels[16] = (adjustedServos[11] >> 7) | (adjustedServos[12] << 4);
    sbusPacket.channels[17] = (adjustedServos[12] >> 4) | (adjustedServos[13] << 7);
    sbusPacket.channels[18] = (adjustedServos[13] >> 1);
    sbusPacket.channels[19] = (adjustedServos[13] >> 9) | (adjustedServos[14] << 2);
    sbusPacket.channels[20] = (adjustedServos[14] >> 6) | (adjustedServos[15] << 5);
    sbusPacket.channels[21] = (adjustedServos[15] >> 3);
    sbusPacket.flags = 0;
    if (adjustedServos[16] > 1024) {
        sbusPacket.flags |= 0x1;
    }
    if (adjustedServos[17] > 1024) {
        sbusPacket.flags |= 0x2;
    }
    if (failsafeEngaged) {
        sbusPacket.flags |= SBUS_FAILSAFE;
    }
    DCH3CONbits.CHEN = 1;
    if (settings.numSBusOutputs > 1) {
        DCH4CONbits.CHEN = 1;
    }
}

void processSBusPacket(uint8_t *buffer) {
    rawServoPositions[0] = ((uint16_t) buffer[1] | (uint16_t) buffer[2] << 8) & 0x7ff;
    rawServoPositions[1] = ((uint16_t) buffer[2] >> 3 | (uint16_t) buffer[3] << 5) & 0x7ff;
    rawServoPositions[2] = ((uint16_t) buffer[3] >> 6 | (uint16_t) buffer[4] << 2
            | (uint16_t) buffer[5] << 10) & 0x7ff;
    rawServoPositions[3] = ((uint16_t) buffer[5] >> 1 | (uint16_t) buffer[6] << 7) & 0x7ff;
    rawServoPositions[4] = ((uint16_t) buffer[6] >> 4 | (uint16_t) buffer[7] << 4) & 0x7ff;
    rawServoPositions[5] = ((uint16_t) buffer[7] >> 7 | (uint16_t) buffer[8] << 1
            | (uint16_t) buffer[9] << 9) & 0x7ff;
    rawServoPositions[6] = ((uint16_t) buffer[9] >> 2 | (uint16_t) buffer[10] << 6) &0x7ff;
    rawServoPositions[7] = ((uint16_t) buffer[10] >> 5 | (uint16_t) buffer[11] << 3) & 0x7ff;
    rawServoPositions[8] = ((uint16_t) buffer[12] | (uint16_t) buffer[13] << 8) &0x7ff;
    rawServoPositions[9] = ((uint16_t) buffer[13] >> 3 | (uint16_t) buffer[14] << 5) & 0x7ff;
    rawServoPositions[10] = ((uint16_t) buffer[14] >> 6 | (uint16_t) buffer[15] << 2
            | (uint16_t) buffer[16] << 10) & 0x7ff;
    rawServoPositions[11] = ((uint16_t) buffer[16] >> 1 | (uint16_t) buffer[17] << 7) & 0x7ff;
    rawServoPositions[12] = ((uint16_t) buffer[17] >> 4 | (uint16_t) buffer[18] << 4) & 0x7ff;
    rawServoPositions[13] = ((uint16_t) buffer[18] >> 7 | (uint16_t) buffer[19] << 1
            | (uint16_t) buffer[20] << 9) &0x7ff;
    rawServoPositions[14] = ((uint16_t) buffer[20] >> 2 | (uint16_t) buffer[21] << 6) & 0x7ff;
    rawServoPositions[15] = ((uint16_t) buffer[21] >> 5 | (uint16_t) buffer[22] << 3) & 0x7ff;
    if (buffer[23] & 0x01) {
        rawServoPositions[16] = 2047;
    } else {
        rawServoPositions[16] = 0;
    }
    if (buffer[23] & 0x02) {
        rawServoPositions[17] = 2047;
    } else {
        rawServoPositions[17] = 0;
    }
}