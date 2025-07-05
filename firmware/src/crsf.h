/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: crsf.h                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: ExpressLRS (CRSF) functions        //
/////////////////////////////////////////////////////
#ifndef CRSF_H
#define	CRSF_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_CRSF_PACKET     64
#define CRSF_CHANNEL_PACKET_LEN  22  //22 bytes channels, leave off the 1 byte CRC

    enum {
        CRSF_ADDRESS_BROADCAST = 0x00,
        CRSF_ADDRESS_USB = 0x10,
        CRSF_ADDRESS_TBS_CORE_PNP_PRO = 0x80,
        CRSF_ADDRESS_RESERVED1 = 0x8A,
        CRSF_ADDRESS_CURRENT_SENSOR = 0xC0,
        CRSF_ADDRESS_GPS = 0xC2,
        CRSF_ADDRESS_TBS_BLACKBOX = 0xC4,
        CRSF_ADDRESS_FLIGHT_CONTROLLER = 0xC8,
        CRSF_ADDRESS_RESERVED2 = 0xCA,
        CRSF_ADDRESS_RACE_TAG = 0xCC,
        CRSF_ADDRESS_RADIO_TRANSMITTER = 0xEA,
        CRSF_ADDRESS_CRSF_RECEIVER = 0xEC,
        CRSF_ADDRESS_CRSF_TRANSMITTER = 0xEE,
    };

    enum {
        CRSF_FRAMETYPE_GPS = 0x02,
        CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
        CRSF_FRAMETYPE_BARO_ALTITUDE = 0x09,
        CRSF_FRAMETYPE_LINK_STATISTICS = 0x14,
        CRSF_FRAMETYPE_OPENTX_SYNC = 0x10,
        CRSF_FRAMETYPE_RADIO_ID = 0x3A,
        CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,
        CRSF_FRAMETYPE_ATTITUDE = 0x1E,
        CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,
        // Extended Header Frames, range: 0x28 to 0x96
        CRSF_FRAMETYPE_DEVICE_PING = 0x28,
        CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
        CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY = 0x2B,
        CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
        CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D,
        CRSF_FRAMETYPE_COMMAND = 0x32,
        // MSP commands
        CRSF_FRAMETYPE_MSP_REQ = 0x7A, // response request using msp sequence as command
        CRSF_FRAMETYPE_MSP_RESP = 0x7B, // reply with 58 byte chunked binary
        CRSF_FRAMETYPE_MSP_WRITE = 0x7C, // write with 8 byte chunked binary (OpenTX outbound telemetry buffer limit)
    };

    typedef struct __attribute__((packed)) {
        uint8_t address;
        uint8_t length;
        uint8_t type;
        uint8_t data[0];
    }
    CRSF_Frame;

    typedef struct __attribute__((packed)) {
        unsigned ch0 : 11;
        unsigned ch1 : 11;
        unsigned ch2 : 11;
        unsigned ch3 : 11;
        unsigned ch4 : 11;
        unsigned ch5 : 11;
        unsigned ch6 : 11;
        unsigned ch7 : 11;
        unsigned ch8 : 11;
        unsigned ch9 : 11;
        unsigned ch10 : 11;
        unsigned ch11 : 11;
        unsigned ch12 : 11;
        unsigned ch13 : 11;
        unsigned ch14 : 11;
        unsigned ch15 : 11;
    }
    CRSF_ChannelData;

    typedef struct __attribute__((packed)) {
        uint8_t uplink_RSSI_1;
        uint8_t uplink_RSSI_2;
        uint8_t uplink_Link_quality;
        int8_t uplink_SNR;
        uint8_t active_antenna;
        uint8_t rf_Mode;
        uint8_t uplink_TX_Power;
        uint8_t downlink_RSSI;
        uint8_t downlink_Link_quality;
        int8_t downlink_SNR;
    }
    CRSF_LinkStatistics;

    typedef struct __attribute__((packed)) {
        int16_t pitch; // radians * 10000
        int16_t roll; // radians * 10000
        int16_t yaw; // radians * 10000
    }
    CRSF_Sensor_Attitude;

    typedef struct __attribute__((packed)) {
        char flight_mode[16];
    }
    CRSF_Sensor_FlightMode;

    extern volatile __attribute__((coherent, aligned(4))) uint8_t crsfPacket[MAX_CRSF_PACKET];
    void initCRSF(void);
    void CRSF_RX_InterruptHandler(void);
    void CRSFPacketDone(DMAC_TRANSFER_EVENT status, uintptr_t contextHandle);
    void startCRSFPacket(uint32_t status, uintptr_t context);
    void processCRSFPacket(uint8_t *buffer);
    void sendModeTelemetry(int mode);
    void sendAttitudeTelemetry(float roll, float pitch, float yaw);
    bool telemetryBusy(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CRSF_H */

