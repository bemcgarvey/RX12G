#include <stdint.h>
#include "RX12G.h"

uint32_t calculateCRC(void *data, int len) {
    uint32_t crc = 0x12345678;
    uint8_t *p = (uint8_t *)data;
    for (int i = 0; i < len; ++i) {
        crc <<= 1;
        crc ^= *p;
        ++p;
    }
    return crc;
}
