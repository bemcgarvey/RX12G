/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: settings.h                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: rx and gyro settings               //
/////////////////////////////////////////////////////

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_CHANNELS    20
    
//minimum gain for modes that override the sticks
// if gain is too low in these modes there will be a loss of control    
#define MIN_GAIN        0.20

    enum {
        AILERON_INDEX = 0, ELEVATOR_INDEX = 1, RUDDER_INDEX = 2,
        AILERON2_INDEX = 3, ELEVATOR2_INDEX = 4
    };

    enum {
        AILERON_MASK = 0x01, ELEVATOR_MASK = 0x02, RUDDER_MASK = 0x04,
        AILERON2_MASK = 0x08, ELEVATOR2_MASK = 0x10
    };

    enum {
        ROLL_INDEX = 0, PITCH_INDEX = 1, YAW_INDEX = 2
    };

    typedef enum {
        NORMAL_FAILSAFE = 0, PRESET_FAILSAFE = 1
    } FailsafeType;

    typedef enum {
        FLAT_ORIENTATION = 0, INVERTED_ORIENTATION = 1, LEFT_DOWN_ORIENTATION = 2,
        RIGHT_DOWN_ORIENTATION = 3
    } OrientationType;

    //ELEVON_A is both servos rotating the same direction for roll/aileron
    //ELEVON_B is both servos rotating the same direction for pitch/elevator 
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
    } PID;

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
        PID rollPID;
        PID pitchPID;
        PID yawPID;
    } Settings;

    extern uint16_t channelPresets[MAX_CHANNELS];

    extern Settings settings;

    uint32_t calculateCRC(void *data, int len);
    bool loadSettings(void);
    bool saveSettings(void);
    bool loadPresets(void);
    bool savePresets(void);
    void loadDefaultSettings(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */

