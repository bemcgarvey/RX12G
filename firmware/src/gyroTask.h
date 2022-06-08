/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: gyroTask.h                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: gyro and stability code            //
/////////////////////////////////////////////////////

#ifndef GYROTASK_H
#define	GYROTASK_H

#include "settings.h"

#ifdef	__cplusplus
extern "C" {
#endif

    extern FlightModeType currentFlightMode;
    extern volatile uint16_t rawServoPositions[MAX_CHANNELS];
    extern float rollGain;
    extern float pitchGain;
    extern float yawGain;
    extern uint16_t rpyCorrections[3];

    //IMU data indices 
#define IMU_GYRO_X      0
#define IMU_GYRO_Y      1
#define IMU_GYRO_Z      2
#define IMU_ACCEL_X      3
#define IMU_ACCEL_Y      4
#define IMU_ACCEL_Z      5
    extern int16_t imuData[6];

    void gyroTask(void *pvParameters);

#ifdef	__cplusplus
}
#endif

#endif	/* GYROTASK_H */

