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
    GET_SENSORS = 0x05,
    CMD_ACK = 0x80,
    CMD_NACK = 0x7f,
    REBOOT = 0x88
};

//Settings
enum {
    NORMAL_FAILSAFE = 0, PRESET_FAILSAFE = 1
};

#define MAX_CHANNELS    20

typedef struct {
    uint32_t outputHz;
    uint32_t sBusPeriodMs;
    uint32_t numSBusOutputs;
    uint32_t failsafeType;
} Settings;

uint32_t calculateCRC(void *data, int len);

#endif // RX12G_H
