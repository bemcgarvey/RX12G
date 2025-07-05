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
    GET_GAINS = 0x06,
    ENABLE_OFFSETS = 0x07,
    DISABLE_OFFSETS = 0x08,
    GET_ATTITUDE = 0x09,
    CMD_ACK = 0x80,
    CMD_NACK = 0x7f,
    REBOOT = 0x88,
    BIND = 0x89,
    BOOTLOAD = 0x90
};

//Settings
#define MAX_CHANNELS    20

    enum {
        AILERON_INDEX = 0, ELEVATOR_INDEX = 1, RUDDER_INDEX = 2,
        AILERON2_INDEX = 3, ELEVATOR2_INDEX = 4
    };

    enum {
        AILERON_MASK = 0x01, ELEVATOR_MASK = 0x02, RUDDER_MASK = 0x04,
        AILERON2_MASK = 0x08, ELEVATOR2_MASK = 0x10
    };

    typedef enum {
        NORMAL_FAILSAFE = 0, PRESET_FAILSAFE = 1
    } FailsafeType;

    typedef enum {
        FLAT_ORIENTATION = 0, INVERTED_ORIENTATION = 1, LEFT_DOWN_ORIENTATION = 2,
        RIGHT_DOWN_ORIENTATION = 3
    } OrientationType;

    typedef enum {
        ONE_AILERON = 0, TWO_AILERON = 1, ELEVON_A = 2, ELEVON_B = 3
    } WingType;

    typedef enum {
        ONE_ELEVATOR = 0, TWO_ELEVATOR = 1, V_TAIL = 2
    } TailType;

    typedef enum {
        OFF_MODE = 0, NORMAL_MODE = 1, AUTO_LEVEL_MODE = 2, TRAINER_MODE = 3,
        ATTITUDE_LOCK_MODE = 4, LAUNCH_ASSIST_MODE = 5, ANGLE_MODE = 6, CUSTOM_MODE_1 = 7,
        CUSTOM_MODE_2 = 8
    } FlightModeType;

    typedef enum {
        GAIN_CURVE_NORMAL = 0, GAIN_CURVE_FLAT = 1, GAIN_CURVE_STEEP = 2
    } GainCurveType;

    enum {CHANNEL_ORDER_TAER = 0x00, CHANNEL_ORDER_AETR = 0xae};

    enum {SAT_TYPE_DSMX = 0, SAT_TYPE_SBUS = 1, SAT_TYPE_CRSF = 2};

    typedef struct {
        float _P;
        float _I;
        float _D;
        float _maxI;
    } _PID;

    typedef struct {
        uint8_t aileronMode;
        uint8_t elevatorMode;
        uint8_t rudderMode;
    } CustomModeType;

    enum {NORMAL_RX_MODE = 0, RX_ONLY_MODE = 'X'};

    typedef struct {
        //rx settings
        uint8_t outputHz;
        uint8_t sBusPeriodMs;
        uint8_t numSBusOutputs;
        uint8_t failsafeType;
        uint8_t rxOnly;
        uint8_t channelOrder;
        uint8_t satType;
        //gyro settings
        uint8_t gyroOrientation;
        uint8_t wingType;
        uint8_t tailType;
        uint8_t gyroEnabledFlags;
        uint8_t gyroReverseFlags;
        uint8_t numFlightModes;
        uint8_t flightModeChannel;
        uint8_t flightModes[6];
        uint8_t deadbands[3];
        CustomModeType customMode1;
        CustomModeType customMode2;
        uint8_t normalGains[3];
        uint8_t levelGains[2];
        uint8_t lockGains[3];
        uint8_t gainChannels[3];
        uint8_t gainCurves[3];
        uint8_t rollLimit;
        uint8_t pitchLimit;
        uint8_t takeoffPitch;
        uint8_t levelOffsets[3];
        uint8_t aileron2Channel;
        uint8_t elevator2Channel;
        uint16_t minTravelLimits[5];
        uint16_t maxTravelLimits[5];
        uint16_t maxRollRate;
        uint16_t maxPitchRate;
        uint16_t maxYawRate;
        _PID rollPID;
        _PID pitchPID;
        _PID yawPID;
    } Settings;

//Sensor scaling
#define mg_PER_LSB      0.122
#define mdps_PER_LSB    70
//Attitude structures
    typedef union {
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float roll;
            float pitch;
            float yaw;
        };
        struct {
            float rollRate;
            float pitchRate;
            float yawRate;
        };
    } Vector;

    typedef struct {
        Vector ypr;
        Vector gyroRatesDeg;
    } AttitudeData;

uint32_t calculateCRC(void *data, int len);

#endif // RX12G_H
