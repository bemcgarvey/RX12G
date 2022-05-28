#ifndef RX12G_H
#define RX12G_H

#define VID 0x4d63
#define PID 0x1100

#include <stdint.h>
//USB commands
enum {
    GET_VERSION = 0xf0,
    GET_SETTINGS = 0x01,
    SAVE_SETTINGS = 0x02,
    GET_CHANNELS = 0x03,
    SET_PRESETS = 0x04,
    CMD_ACK = 0x80,
    CMD_NACK = 0x7f
};

//Settings
enum {
    NORMAL_FAILSAFE = 0, PRESET_FAILSAFE = 1
};

#define MAX_CHANNELS    20

typedef struct __attribute__((packed)) {
    uint16_t outputHz;
    uint16_t channelPresets[MAX_CHANNELS];
    uint8_t sBusPeriodMs;
    uint8_t numSBusOutputs;
    uint8_t failsafeType;
} Settings;

uint32_t calculateCRC(void *data, int len);

#endif // RX12G_H
